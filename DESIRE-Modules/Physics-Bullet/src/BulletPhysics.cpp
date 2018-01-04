#include "stdafx.h"
#include "BulletPhysics.h"
#include "BulletPhysicsComponent.h"
#include "BulletDebugDraw.h"
#include "BulletVectormathExt.h"

#include "Core/Timer.h"
#include "Component/ScriptComponent.h"
#include "Physics/Collision.h"
#include "Scene/Object.h"

#include "btBulletDynamicsCommon.h"
#include "BulletCollision/NarrowPhaseCollision/btRaycastCallback.h"

BulletPhysics::BulletPhysics()
{
	collisionConfiguration = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(collisionConfiguration);

	btVector3 worldMin(-1000.0f, -1000.0f, -1000.0f);
	btVector3 worldMax(1000.0f, 1000.0f, 1000.0f);
	broadphase = new btAxisSweep3(worldMin, worldMax);
	constraintSolver = new btSequentialImpulseConstraintSolver();
	dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, constraintSolver, collisionConfiguration);
	dynamicsWorld->setGravity(btVector3(0.0f, -10.0f, 0.0f));
	dynamicsWorld->setInternalTickCallback(BulletPhysics::SimulationTickCallback, this);

	btDispatcherInfo& dispatchInfo = dynamicsWorld->getDispatchInfo();
	dispatchInfo.m_useContinuous = true;

	btContactSolverInfo& solverInfo = dynamicsWorld->getSolverInfo();
	// The following two lines increase the performance, with a little bit of loss of quality
	solverInfo.m_solverMode |= SOLVER_ENABLE_FRICTION_DIRECTION_CACHING;
	solverInfo.m_numIterations = 4;

	class DebugDraw *debugDraw = nullptr;
	if(debugDraw != nullptr)
	{
		blletDebugDraw = new BulletDebugDraw(debugDraw);
		blletDebugDraw->setDebugMode(btIDebugDraw::DBG_DrawWireframe | btIDebugDraw::DBG_DrawNormals);
	}
	dynamicsWorld->setDebugDrawer(blletDebugDraw);
}

BulletPhysics::~BulletPhysics()
{
	delete blletDebugDraw;

	delete dynamicsWorld;
	delete constraintSolver;
	delete broadphase;
	delete dispatcher;
	delete collisionConfiguration;
}

void BulletPhysics::Update()
{
	if(blletDebugDraw != nullptr)
	{
		blletDebugDraw->debugDraw->Reset();
	}

	const float deltaT = Timer::Get()->GetSecDelta();
	dynamicsWorld->stepSimulation(deltaT, 7);
}

PhysicsComponent* BulletPhysics::CreatePhysicsComponentOnObject(Object& object)
{
	BulletPhysicsComponent *component = new BulletPhysicsComponent(object, true);
	return component;
}

bool BulletPhysics::RaycastClosest(const Vector3& p1, const Vector3& p2, PhysicsComponent **o_componentPtr, Vector3 *o_collisionPointPtr, Vector3 *o_collisionNormalPtr, int layerMask)
{
	const btVector3 from = GetBtVector3(p1);
	const btVector3 to = GetBtVector3(p2);
	btCollisionWorld::ClosestRayResultCallback rayCallback(from, to);
	rayCallback.m_collisionFilterGroup = layerMask;
	rayCallback.m_collisionFilterMask = layerMask;
	rayCallback.m_flags = btTriangleRaycastCallback::kF_FilterBackfaces;

	dynamicsWorld->rayTest(from, to, rayCallback);

	if(!rayCallback.hasHit())
	{
		return false;
	}

	if(o_componentPtr != nullptr)
	{
		const btRigidBody *body = btRigidBody::upcast(rayCallback.m_collisionObject);
		if(body != nullptr && body->hasContactResponse())
		{
			*o_componentPtr = static_cast<PhysicsComponent*>(body->getUserPointer());
		}
		else
		{
			*o_componentPtr = nullptr;
		}
	}

	if(o_collisionPointPtr != nullptr)
	{
		*o_collisionPointPtr = GetVector3(rayCallback.m_hitPointWorld);
	}

	if(o_collisionNormalPtr != nullptr)
	{
		*o_collisionNormalPtr = GetVector3(rayCallback.m_hitNormalWorld);
	}

	return true;
}

bool BulletPhysics::RaycastAny(const Vector3& p1, const Vector3& p2, int layerMask)
{
	ASSERT(false && "TODO implement");
	return false;
}

int BulletPhysics::RaycastAll(const Vector3& p1, const Vector3& p2, int maxCount, PhysicsComponent **o_components, Vector3 *o_collisionPoints, Vector3 *o_collisionNormals, int layerMask)
{
	const btVector3 from = GetBtVector3(p1);
	const btVector3 to = GetBtVector3(p2);
	btCollisionWorld::AllHitsRayResultCallback rayCallback(from, to);
	rayCallback.m_collisionFilterGroup = layerMask;
	rayCallback.m_collisionFilterMask = layerMask;
	rayCallback.m_flags = btTriangleRaycastCallback::kF_FilterBackfaces;

	dynamicsWorld->rayTest(from, to, rayCallback);

	if(!rayCallback.hasHit())
	{
		return 0;
	}

	const int count = std::min(maxCount, rayCallback.m_collisionObjects.size());
	for(int i = 0; i < count; ++i)
	{
		if(o_components != nullptr)
		{
			const btRigidBody *body = btRigidBody::upcast(rayCallback.m_collisionObjects[i]);
			if(body != nullptr && body->hasContactResponse())
			{
				o_components[i] = static_cast<PhysicsComponent*>(body->getUserPointer());
			}
			else
			{
				o_components[i] = nullptr;
			}
		}

		if(o_collisionPoints != nullptr)
		{
			o_collisionPoints[i] = GetVector3(rayCallback.m_hitPointWorld[i]);
		}

		if(o_collisionNormals != nullptr)
		{
			o_collisionNormals[i] = GetVector3(rayCallback.m_hitNormalWorld[i]);
		}
	}

	return count;
}

int BulletPhysics::GetMaskForCollisionLayer(EPhysicsCollisionLayer layer) const
{
	ASSERT(layer < EPhysicsCollisionLayer::NUM);
	return collisionMasks[(size_t)layer];
}

void BulletPhysics::SimulationTickCallback(btDynamicsWorld *world, float timeStep)
{
	DESIRE_UNUSED(timeStep);

	BulletPhysics *physics = static_cast<BulletPhysics*>(world->getWorldUserInfo());

	const int numManifolds = physics->dispatcher->getNumManifolds();
	for(int manifoldIdx = 0; manifoldIdx < numManifolds; ++manifoldIdx)
	{
		Collision collision;

		const btPersistentManifold *manifold = physics->dispatcher->getManifoldByIndexInternal(manifoldIdx);
		const btRigidBody *body0 = static_cast<const btRigidBody*>(manifold->getBody0());
		const btRigidBody *body1 = static_cast<const btRigidBody*>(manifold->getBody1());
		collision.pointCount = std::min(manifold->getNumContacts(), Collision::MAX_CONTACT_POINTS);
		for(int i = 0; i < collision.pointCount; ++i)
		{
			const btManifoldPoint& pt = manifold->getContactPoint(i);
			if(pt.getDistance() < 0.0f)
			{
				collision.contactPoints[i] = GetVector3(pt.getPositionWorldOnB());
				collision.contactNormals[i] = GetVector3(pt.m_normalWorldOnB);
			}
		}

		BulletPhysicsComponent *component0 = static_cast<BulletPhysicsComponent*>(body0->getUserPointer());
		BulletPhysicsComponent *component1 = static_cast<BulletPhysicsComponent*>(body1->getUserPointer());

		ScriptComponent *scriptComponent = component0->GetObject().GetComponent<ScriptComponent>();
		if(scriptComponent != nullptr)
		{
			collision.component = component0;
			collision.incomingComponent = component1;
			scriptComponent->Call("OnCollisionEnter", &collision);
		}

		scriptComponent = component1->GetObject().GetComponent<ScriptComponent>();
		if(scriptComponent != nullptr)
		{
			collision.component = component1;
			collision.incomingComponent = component0;
			scriptComponent->Call("OnCollisionEnter", &collision);
		}
	}
}
