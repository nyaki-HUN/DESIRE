#include "BulletPhysics.h"
#include "BulletPhysicsComponent.h"
#include "BulletDebugDraw.h"
#include "BulletVectormathExt.h"

#include "Engine/Core/assert.h"
#include "Engine/Component/ScriptComponent.h"
#include "Engine/Scene/Object.h"

#include "btBulletDynamicsCommon.h"
#include "BulletCollision/NarrowPhaseCollision/btRaycastCallback.h"

#include <algorithm>		// for std::min

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
		bulletDebugDraw = new BulletDebugDraw(*debugDraw);
		bulletDebugDraw->setDebugMode(btIDebugDraw::DBG_DrawWireframe | btIDebugDraw::DBG_DrawNormals);
	}
	dynamicsWorld->setDebugDrawer(bulletDebugDraw);
}

BulletPhysics::~BulletPhysics()
{
	delete bulletDebugDraw;

	delete dynamicsWorld;
	delete constraintSolver;
	delete broadphase;
	delete dispatcher;
	delete collisionConfiguration;
}

void BulletPhysics::Update(float deltaTime)
{
	if(bulletDebugDraw != nullptr)
	{
		bulletDebugDraw->debugDraw.Reset();
	}

	dynamicsWorld->stepSimulation(deltaTime, 7, fixedUpdateDeltaTime);

	UpdateComponents();
}

PhysicsComponent& BulletPhysics::CreatePhysicsComponentOnObject(Object& object)
{
	BulletPhysicsComponent& component = object.AddComponent<BulletPhysicsComponent>( true);
	return component;
}

void BulletPhysics::SetGravity(const Vector3& gravity)
{
	dynamicsWorld->setGravity(GetBtVector3(gravity));
}

Vector3 BulletPhysics::GetGravity() const
{
	const btVector3& gravity = dynamicsWorld->getGravity();
	return GetVector3(gravity);
}

Collision BulletPhysics::RaycastClosest(const Vector3& p1, const Vector3& p2, int layerMask)
{
	const btVector3 from = GetBtVector3(p1);
	const btVector3 to = GetBtVector3(p2);
	btCollisionWorld::ClosestRayResultCallback rayCallback(from, to);
	rayCallback.m_collisionFilterGroup = layerMask;
	rayCallback.m_collisionFilterMask = layerMask;
	rayCallback.m_flags = btTriangleRaycastCallback::kF_FilterBackfaces;

	dynamicsWorld->rayTest(from, to, rayCallback);

	Collision collision;
	if(rayCallback.hasHit())
	{
		const btRigidBody *body = btRigidBody::upcast(rayCallback.m_collisionObject);
		PhysicsComponent *component = (body != nullptr && body->hasContactResponse()) ? static_cast<PhysicsComponent*>(body->getUserPointer()) : nullptr;
		collision = Collision(component, GetVector3(rayCallback.m_hitPointWorld), GetVector3(rayCallback.m_hitNormalWorld));
	}

	return collision;
}

bool BulletPhysics::RaycastAny(const Vector3& p1, const Vector3& p2, int layerMask)
{
	ASSERT(false && "TODO implement");
	return false;
}

std::vector<Collision> BulletPhysics::RaycastAll(const Vector3& p1, const Vector3& p2, int layerMask)
{
	const btVector3 from = GetBtVector3(p1);
	const btVector3 to = GetBtVector3(p2);
	btCollisionWorld::AllHitsRayResultCallback rayCallback(from, to);
	rayCallback.m_collisionFilterGroup = layerMask;
	rayCallback.m_collisionFilterMask = layerMask;
	rayCallback.m_flags = btTriangleRaycastCallback::kF_FilterBackfaces;

	dynamicsWorld->rayTest(from, to, rayCallback);

	std::vector<Collision> collisions;
	if(rayCallback.hasHit())
	{
		const int count = rayCallback.m_collisionObjects.size();
		collisions.reserve(count);
		for(int i = 0; i < count; ++i)
		{
			const btRigidBody *body = btRigidBody::upcast(rayCallback.m_collisionObjects[i]);
			PhysicsComponent *component = (body != nullptr && body->hasContactResponse()) ? static_cast<PhysicsComponent*>(body->getUserPointer()) : nullptr;
			collisions.emplace_back(component, GetVector3(rayCallback.m_hitPointWorld[i]), GetVector3(rayCallback.m_hitNormalWorld[i]));
		}
	}

	return collisions;
}

btDynamicsWorld* BulletPhysics::GetWorld() const
{
	return dynamicsWorld;
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
		collision.pointCount = std::min(manifold->getNumContacts(), Collision::k_MaxContactPoints);
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

		collision.component = component0;
		collision.incomingComponent = component1;
		for(ScriptComponent *scriptComponent : component0->GetObject().GetScriptComponents())
		{
			scriptComponent->Call("OnCollisionEnter", &collision);
		}

		collision.component = component1;
		collision.incomingComponent = component0;
		for(ScriptComponent *scriptComponent : component1->GetObject().GetScriptComponents())
		{
			scriptComponent->Call("OnCollisionEnter", &collision);
		}
	}
}
