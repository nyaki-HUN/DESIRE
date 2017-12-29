#pragma once

#include "Core/platform.h"
#include "Physics/IPhysics.h"

DESIRE_DISABLE_WARNINGS
#include "../Externals/Bullet/src/btBulletDynamicsCommon.h"
DESIRE_ENABLE_WARNINGS

class BulletDebugDraw;

class BulletPhysics : public IPhysics
{
public:
	BulletPhysics();
	~BulletPhysics() override;

	void Update() override;

	PhysicsComponent* CreatePhysicsComponent() override;

	bool RaycastClosest(const Vector3& p1, const Vector3& p2, PhysicsComponent **o_componentPtr, Vector3 *o_collisionPointPtr = nullptr, Vector3 *o_collisionNormalPtr = nullptr, int layerMask = IPhysics::MASK_ALL) override;
	bool RaycastAny(const Vector3& p1, const Vector3& p2, int layerMask = IPhysics::MASK_ALL) override;
	int RaycastAll(const Vector3& p1, const Vector3& p2, int maxCount, PhysicsComponent **o_components, Vector3 *o_collisionPoints = nullptr, Vector3 *o_collisionNormals = nullptr, int layerMask = IPhysics::MASK_ALL) override;

	int GetMaskForCollisionLayer(EPhysicsCollisionLayer layer) const;

	btDiscreteDynamicsWorld *dynamicsWorld;

private:
	static void SimulationTickCallback(btDynamicsWorld *world, float timeStep);

	BulletDebugDraw *blletDebugDraw;

	btDefaultCollisionConfiguration *collisionConfiguration;
	btCollisionDispatcher *dispatcher;
	btAxisSweep3 *broadphase;
	btSequentialImpulseConstraintSolver *constraintSolver;
};
