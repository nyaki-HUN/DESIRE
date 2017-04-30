#include "stdafx.h"
#include "Physics/Bullet/BulletPhysics.h"
#include "Physics/Bullet/BulletPhysicsComponent.h"
#include "Physics/Bullet/BulletDebugDraw.h"
#include "Physics/Bullet/BulletVectormathExt.h"
#include "Physics/Collision.h"
#include "Core/Timer.h"

#include "Physics-Bullet/src/BulletCollision/NarrowPhaseCollision/btRaycastCallback.h"

BulletPhysics::BulletPhysics()
	: dynamicsWorld(nullptr)
	, collisionConfiguration(nullptr)
	, dispatcher(nullptr)
	, broadphase(nullptr)
	, constraintSolver(nullptr)
	, blletDebugDraw(nullptr)
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

	// Setup collision masks
	memset(collisionMasks, 0xff, sizeof(collisionMasks));
	for(size_t i = 0; i < DESIRE_ASIZEOF(collisionMasks); i++)
	{
		SetCollisionEnabled((EPhysicsCollisionGroup)i, EPhysicsCollisionGroup::WHEEL, false);
		SetCollisionEnabled((EPhysicsCollisionGroup)i, EPhysicsCollisionGroup::NO_COLLISION, false);
	}
	SetCollisionEnabled(EPhysicsCollisionGroup::DEFAULT, EPhysicsCollisionGroup::WHEEL, true);
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

PhysicsComponent* BulletPhysics::CreatePhysicsComponent()
{
	BulletPhysicsComponent *component = new BulletPhysicsComponent(true);
	return component;
}

void BulletPhysics::SetCollisionEnabled(EPhysicsCollisionGroup a, EPhysicsCollisionGroup b, bool enabled)
{
	const size_t groupA = (size_t)a;
	const size_t groupB = (size_t)b;

	ASSERT(groupA < DESIRE_ASIZEOF(collisionMasks));
	ASSERT(groupB < DESIRE_ASIZEOF(collisionMasks));

	if(enabled)
	{
		collisionMasks[groupA] |= (1 << groupB);
		collisionMasks[groupB] |= (1 << groupA);
	}
	else
	{
		collisionMasks[groupA] &= ~(1 << groupB);
		collisionMasks[groupB] &= ~(1 << groupA);
	}
}

int16_t BulletPhysics::GetCollisionMaskForGroup(EPhysicsCollisionGroup group) const
{
	ASSERT((size_t)group < DESIRE_ASIZEOF(collisionMasks));
	return collisionMasks[(size_t)group];
}

bool BulletPhysics::RayTest(const Vector3& startPoint, const Vector3& direction, Vector3 *o_hitpoint, PhysicsComponent **o_component, int collisionGroupMask)
{
	const btVector3 from = GetBtVector3(startPoint);
	const btVector3 to = GetBtVector3(startPoint + direction * 10000.0f);
	btCollisionWorld::ClosestRayResultCallback rayCallback(from, to);
	rayCallback.m_collisionFilterGroup = (int16_t)collisionGroupMask;
	rayCallback.m_collisionFilterMask = (int16_t)collisionGroupMask;
	rayCallback.m_flags = btTriangleRaycastCallback::kF_FilterBackfaces;

	dynamicsWorld->rayTest(from, to, rayCallback);

	if(!rayCallback.hasHit())
	{
		return false;
	}

	if(o_hitpoint != nullptr)
	{
		*o_hitpoint = GetVector3(rayCallback.m_hitPointWorld);
	}

	if(o_component != nullptr)
	{
		const btRigidBody *body = btRigidBody::upcast(rayCallback.m_collisionObject);
		if(body != nullptr && body->hasContactResponse())
		{
			*o_component = static_cast<PhysicsComponent*>(body->getUserPointer());
		}
		else
		{
			*o_component = nullptr;
		}
	}
	return true;
}

void BulletPhysics::SimulationTickCallback(btDynamicsWorld *world, float timeStep)
{
	DESIRE_UNUSED(timeStep);

	BulletPhysics *physics = static_cast<BulletPhysics*>(world->getWorldUserInfo());

	const int numManifolds = physics->dispatcher->getNumManifolds();
	for(int i = 0; i < numManifolds; i++)
	{
		Collision collision;

		const btPersistentManifold *manifold = physics->dispatcher->getManifoldByIndexInternal(i);
		const btRigidBody *body0 = static_cast<const btRigidBody*>(manifold->getBody0());
		const btRigidBody *body1 = static_cast<const btRigidBody*>(manifold->getBody1());
		int numContacts = manifold->getNumContacts();
		numContacts = std::min(numContacts, Collision::MAX_CONTACT_POINTS);
		for(int j = 0; j < numContacts; ++j)
		{
			const btManifoldPoint& pt = manifold->getContactPoint(i);
			if(pt.getDistance() < 0.0f)
			{
				collision.contactPoints[i] = GetVector3(pt.getPositionWorldOnB());
				collision.force[i] = GetVector3(pt.m_normalWorldOnB * pt.m_appliedImpulse);
			}
		}

		BulletPhysicsComponent *component0 = static_cast<BulletPhysicsComponent*>(body0->getUserPointer());
		BulletPhysicsComponent *component1 = static_cast<BulletPhysicsComponent*>(body1->getUserPointer());
		collision.otherComponent = component1;
//		component0->OnContact(collision);
		collision.otherComponent = component0;
//		component1->OnContact(collision);
	}
}
