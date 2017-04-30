#pragma once

#include "Physics/IPhysics.h"

DESIRE_DISABLE_WARNINGS
#include "Physics-Bullet/src/btBulletDynamicsCommon.h"
DESIRE_ENABLE_WARNINGS

class BulletDebugDraw;

class BulletPhysics : public IPhysics
{
public:
	BulletPhysics();
	~BulletPhysics();

	void Update() override;

	PhysicsComponent* CreatePhysicsComponent() override;

	void SetCollisionEnabled(EPhysicsCollisionGroup a, EPhysicsCollisionGroup b, bool enabled) override;
	int16_t GetCollisionMaskForGroup(EPhysicsCollisionGroup group) const;

	bool RayTest(const Vector3& startPoint, const Vector3& direction, Vector3 *o_hitpoint = nullptr, PhysicsComponent **o_component = nullptr, int collisionGroupMask = 0xffffffff) override;

	btDiscreteDynamicsWorld *dynamicsWorld;

private:
	static void SimulationTickCallback(btDynamicsWorld *world, float timeStep);

	int16_t collisionMasks[16];
	BulletDebugDraw *blletDebugDraw;

	btDefaultCollisionConfiguration *collisionConfiguration;
	btCollisionDispatcher *dispatcher;
	btAxisSweep3 *broadphase;
	btSequentialImpulseConstraintSolver *constraintSolver;
};
