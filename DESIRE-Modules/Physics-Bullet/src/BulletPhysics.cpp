#include "stdafx_Bullet.h"
#include "BulletPhysics.h"

#include "BulletCallbacks.h"
#include "BulletDebugDraw.h"
#include "BulletPhysicsComponent.h"

#include "Engine/Core/Object.h"

#include "BulletCollision/NarrowPhaseCollision/btRaycastCallback.h"

BulletPhysics::BulletPhysics()
{
	btAlignedAllocSetCustom(&BulletCallbacks::MallocWrapper, &BulletCallbacks::FreeWrapper);
	btAlignedAllocSetCustomAligned(&BulletCallbacks::AlignedMallocWrapper, &BulletCallbacks::FreeWrapper);

	m_spCollisionConfiguration = std::make_unique<btDefaultCollisionConfiguration>();
	m_spDispatcher = std::make_unique<btCollisionDispatcher>(m_spCollisionConfiguration.get());

	btVector3 worldMin(-1000.0f, -1000.0f, -1000.0f);
	btVector3 worldMax(1000.0f, 1000.0f, 1000.0f);
	m_spBroadphase = std::make_unique<btAxisSweep3>(worldMin, worldMax);
	m_spConstraintSolver = std::make_unique<btSequentialImpulseConstraintSolver>();
	m_spDynamicsWorld = std::make_unique<btDiscreteDynamicsWorld>(m_spDispatcher.get(), m_spBroadphase.get(), m_spConstraintSolver.get(), m_spCollisionConfiguration.get());
	m_spDynamicsWorld->setGravity(btVector3(0.0f, -10.0f, 0.0f));
	m_spDynamicsWorld->setInternalTickCallback(&BulletCallbacks::SimulationTickCallback, this);

	btDispatcherInfo& dispatchInfo = m_spDynamicsWorld->getDispatchInfo();
	dispatchInfo.m_useContinuous = true;

	btContactSolverInfo& solverInfo = m_spDynamicsWorld->getSolverInfo();
	// The following two lines increase the performance, with a little bit of loss of quality
	solverInfo.m_solverMode |= SOLVER_ENABLE_FRICTION_DIRECTION_CACHING;
	solverInfo.m_numIterations = 4;

	class DebugDraw* debugDraw = nullptr;
	if(debugDraw != nullptr)
	{
		m_spBulletDebugDraw = std::make_unique<BulletDebugDraw>(*debugDraw);
		m_spBulletDebugDraw->setDebugMode(btIDebugDraw::DBG_DrawWireframe | btIDebugDraw::DBG_DrawNormals);
	}
	m_spDynamicsWorld->setDebugDrawer(m_spBulletDebugDraw.get());
}

BulletPhysics::~BulletPhysics()
{
	m_spBulletDebugDraw = nullptr;
	m_spDynamicsWorld->setDebugDrawer(nullptr);

	m_spDynamicsWorld = nullptr;
	m_spConstraintSolver = nullptr;
	m_spBroadphase = nullptr;
	m_spDispatcher = nullptr;
	m_spCollisionConfiguration = nullptr;
}

void BulletPhysics::Update(float deltaTime)
{
	if(m_spBulletDebugDraw != nullptr)
	{
		m_spBulletDebugDraw->m_debugDraw.Reset();
	}

	m_spDynamicsWorld->stepSimulation(deltaTime, 7, m_fixedStepTime);

	UpdateComponents();
}

PhysicsComponent& BulletPhysics::CreatePhysicsComponentOnObject(Object& object)
{
	BulletPhysicsComponent& component = object.AddComponent<BulletPhysicsComponent>(true);
	return component;
}

void BulletPhysics::SetGravity(const Vector3& gravity)
{
	m_spDynamicsWorld->setGravity(GetBtVector3(gravity));
}

Vector3 BulletPhysics::GetGravity() const
{
	const btVector3& gravity = m_spDynamicsWorld->getGravity();
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

	m_spDynamicsWorld->rayTest(from, to, rayCallback);

	Collision collision;
	if(rayCallback.hasHit())
	{
		const btRigidBody* body = btRigidBody::upcast(rayCallback.m_collisionObject);
		PhysicsComponent* component = (body != nullptr && body->hasContactResponse()) ? static_cast<PhysicsComponent*>(body->getUserPointer()) : nullptr;
		collision = Collision(component, GetVector3(rayCallback.m_hitPointWorld), GetVector3(rayCallback.m_hitNormalWorld));
	}

	return collision;
}

bool BulletPhysics::RaycastAny(const Vector3& p1, const Vector3& p2, int layerMask)
{
	ASSERT(false && "TODO implement");
	return false;
}

Array<Collision> BulletPhysics::RaycastAll(const Vector3& p1, const Vector3& p2, int layerMask)
{
	const btVector3 from = GetBtVector3(p1);
	const btVector3 to = GetBtVector3(p2);
	btCollisionWorld::AllHitsRayResultCallback rayCallback(from, to);
	rayCallback.m_collisionFilterGroup = layerMask;
	rayCallback.m_collisionFilterMask = layerMask;
	rayCallback.m_flags = btTriangleRaycastCallback::kF_FilterBackfaces;

	m_spDynamicsWorld->rayTest(from, to, rayCallback);

	Array<Collision> collisions;
	if(rayCallback.hasHit())
	{
		const int count = rayCallback.m_collisionObjects.size();
		collisions.Reserve(count);
		for(int i = 0; i < count; ++i)
		{
			const btRigidBody* pBody = btRigidBody::upcast(rayCallback.m_collisionObjects[i]);
			PhysicsComponent* pPhysicsComponent = (pBody != nullptr && pBody->hasContactResponse()) ? static_cast<PhysicsComponent*>(pBody->getUserPointer()) : nullptr;
			collisions.EmplaceAdd(pPhysicsComponent, GetVector3(rayCallback.m_hitPointWorld[i]), GetVector3(rayCallback.m_hitNormalWorld[i]));
		}
	}

	return collisions;
}

btDynamicsWorld& BulletPhysics::GetWorld() const
{
	return *m_spDynamicsWorld;
}
