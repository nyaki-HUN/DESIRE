#pragma once

#include "Engine/Physics/Physics.h"

class BulletCallbacks;
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

	Collision RaycastClosest(const Vector3& p1, const Vector3& p2, int32_t layerMask = Physics::MASK_ALL) override;
	bool RaycastAny(const Vector3& p1, const Vector3& p2, int32_t layerMask = Physics::MASK_ALL) override;
	Array<Collision> RaycastAll(const Vector3& p1, const Vector3& p2, int32_t layerMask = Physics::MASK_ALL) override;

	btDynamicsWorld& GetWorld() const;

private:
	std::unique_ptr<btDiscreteDynamicsWorld> m_spDynamicsWorld;
	std::unique_ptr<btDefaultCollisionConfiguration> m_spCollisionConfiguration;
	std::unique_ptr<btCollisionDispatcher> m_spDispatcher;
	std::unique_ptr<btAxisSweep3> m_spBroadphase;
	std::unique_ptr<btSequentialImpulseConstraintSolver> m_spConstraintSolver;

	std::unique_ptr<BulletDebugDraw> m_spBulletDebugDraw;

	friend BulletCallbacks;
};
