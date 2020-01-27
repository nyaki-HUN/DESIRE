#pragma once

#include "Engine/Physics/Physics.h"

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

	void Update(float deltaTime) override;

	PhysicsComponent& CreatePhysicsComponentOnObject(Object& object) override;

	void SetGravity(const Vector3& gravity) override;
	Vector3 GetGravity() const override;

	Collision RaycastClosest(const Vector3& p1, const Vector3& p2, int layerMask = Physics::MASK_ALL) override;
	bool RaycastAny(const Vector3& p1, const Vector3& p2, int layerMask = Physics::MASK_ALL) override;
	Array<Collision> RaycastAll(const Vector3& p1, const Vector3& p2, int layerMask = Physics::MASK_ALL) override;

	btDynamicsWorld* GetWorld() const;

private:
	static void SimulationTickCallback(btDynamicsWorld* world, float timeStep);

	btDiscreteDynamicsWorld* dynamicsWorld = nullptr;
	btDefaultCollisionConfiguration* collisionConfiguration = nullptr;
	btCollisionDispatcher* dispatcher = nullptr;
	btAxisSweep3* broadphase = nullptr;
	btSequentialImpulseConstraintSolver* constraintSolver = nullptr;

	BulletDebugDraw* bulletDebugDraw = nullptr;
};
