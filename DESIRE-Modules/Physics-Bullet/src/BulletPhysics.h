#pragma once

#include "Physics/Physics.h"

class BulletDebugDraw;
class btDiscreteDynamicsWorld;
class btDynamicsWorld;
class btDefaultCollisionConfiguration;
class btCollisionDispatcher;
class btAxisSweep3;
class btSequentialImpulseConstraintSolver;

class BulletPhysics : public Physics
{
public:
	BulletPhysics();
	~BulletPhysics() override;

	void Update() override;

	PhysicsComponent& CreatePhysicsComponentOnObject(Object& object) override;

	bool RaycastClosest(const Vector3& p1, const Vector3& p2, PhysicsComponent **o_componentPtr, Vector3 *o_collisionPointPtr = nullptr, Vector3 *o_collisionNormalPtr = nullptr, int layerMask = Physics::MASK_ALL) override;
	bool RaycastAny(const Vector3& p1, const Vector3& p2, int layerMask = Physics::MASK_ALL) override;
	int RaycastAll(const Vector3& p1, const Vector3& p2, int maxCount, PhysicsComponent **o_components, Vector3 *o_collisionPoints = nullptr, Vector3 *o_collisionNormals = nullptr, int layerMask = Physics::MASK_ALL) override;

	int GetMaskForCollisionLayer(EPhysicsCollisionLayer layer) const;

	btDiscreteDynamicsWorld *dynamicsWorld = nullptr;

private:
	static void SimulationTickCallback(btDynamicsWorld *world, float timeStep);

	BulletDebugDraw *blletDebugDraw = nullptr;

	btDefaultCollisionConfiguration *collisionConfiguration = nullptr;
	btCollisionDispatcher *dispatcher = nullptr;
	btAxisSweep3 *broadphase = nullptr;
	btSequentialImpulseConstraintSolver *constraintSolver = nullptr;
};
