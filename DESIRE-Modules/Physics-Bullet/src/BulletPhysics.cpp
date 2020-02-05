#include "stdafx_Bullet.h"
#include "BulletPhysics.h"
#include "BulletPhysicsComponent.h"
#include "BulletCallbacks.h"
#include "BulletDebugDraw.h"

#include "Engine/Core/Object.h"

#include "BulletCollision/NarrowPhaseCollision/btRaycastCallback.h"

BulletPhysics::BulletPhysics()
{
	btAlignedAllocSetCustom(&BulletCallbacks::MallocWrapper, &BulletCallbacks::FreeWrapper);
	btAlignedAllocSetCustomAligned(&BulletCallbacks::AlignedMallocWrapper, &BulletCallbacks::FreeWrapper);

	collisionConfiguration = std::make_unique<btDefaultCollisionConfiguration>();
	dispatcher = std::make_unique<btCollisionDispatcher>(collisionConfiguration.get());

	btVector3 worldMin(-1000.0f, -1000.0f, -1000.0f);
	btVector3 worldMax(1000.0f, 1000.0f, 1000.0f);
	broadphase = std::make_unique<btAxisSweep3>(worldMin, worldMax);
	constraintSolver = std::make_unique<btSequentialImpulseConstraintSolver>();
	dynamicsWorld = std::make_unique<btDiscreteDynamicsWorld>(dispatcher.get(), broadphase.get(), constraintSolver.get(), collisionConfiguration.get());
	dynamicsWorld->setGravity(btVector3(0.0f, -10.0f, 0.0f));
	dynamicsWorld->setInternalTickCallback(&BulletCallbacks::SimulationTickCallback, this);

	btDispatcherInfo& dispatchInfo = dynamicsWorld->getDispatchInfo();
	dispatchInfo.m_useContinuous = true;

	btContactSolverInfo& solverInfo = dynamicsWorld->getSolverInfo();
	// The following two lines increase the performance, with a little bit of loss of quality
	solverInfo.m_solverMode |= SOLVER_ENABLE_FRICTION_DIRECTION_CACHING;
	solverInfo.m_numIterations = 4;

	class DebugDraw* debugDraw = nullptr;
	if(debugDraw != nullptr)
	{
		bulletDebugDraw = std::make_unique<BulletDebugDraw>(*debugDraw);
		bulletDebugDraw->setDebugMode(btIDebugDraw::DBG_DrawWireframe | btIDebugDraw::DBG_DrawNormals);
	}
	dynamicsWorld->setDebugDrawer(bulletDebugDraw.get());
}

BulletPhysics::~BulletPhysics()
{
	bulletDebugDraw = nullptr;
	dynamicsWorld->setDebugDrawer(nullptr);

	dynamicsWorld = nullptr;
	constraintSolver = nullptr;
	broadphase = nullptr;
	dispatcher = nullptr;
	collisionConfiguration = nullptr;
}

void BulletPhysics::Update(float deltaTime)
{
	if(bulletDebugDraw != nullptr)
	{
		bulletDebugDraw->debugDraw.Reset();
	}

	dynamicsWorld->stepSimulation(deltaTime, 7, fixedStepTime);

	UpdateComponents();
}

PhysicsComponent& BulletPhysics::CreatePhysicsComponentOnObject(Object& object)
{
	BulletPhysicsComponent& component = object.AddComponent<BulletPhysicsComponent>(true);
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

	dynamicsWorld->rayTest(from, to, rayCallback);

	Array<Collision> collisions;
	if(rayCallback.hasHit())
	{
		const int count = rayCallback.m_collisionObjects.size();
		collisions.Reserve(count);
		for(int i = 0; i < count; ++i)
		{
			const btRigidBody* body = btRigidBody::upcast(rayCallback.m_collisionObjects[i]);
			PhysicsComponent* component = (body != nullptr && body->hasContactResponse()) ? static_cast<PhysicsComponent*>(body->getUserPointer()) : nullptr;
			collisions.EmplaceAdd(component, GetVector3(rayCallback.m_hitPointWorld[i]), GetVector3(rayCallback.m_hitNormalWorld[i]));
		}
	}

	return collisions;
}

btDynamicsWorld* BulletPhysics::GetWorld() const
{
	return dynamicsWorld.get();
}
