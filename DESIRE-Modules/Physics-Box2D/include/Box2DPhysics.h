#pragma once

#include "Engine/Physics/Physics.h"

class b2World;
class b2ContactListener;
class b2DestructionListener;
class b2Body;

class Box2DPhysics : public Physics
{
public:
	Box2DPhysics();
	~Box2DPhysics() override;

	void Update(float deltaTime) override;

	PhysicsComponent& CreatePhysicsComponentOnObject(Object& object) override;

	void SetGravity(const Vector3& gravity) override;
	Vector3 GetGravity() const override;

	Collision RaycastClosest(const Vector3& p1, const Vector3& p2, int layerMask = Physics::MASK_ALL) override;
	bool RaycastAny(const Vector3& p1, const Vector3& p2, int layerMask = Physics::MASK_ALL) override;
	Array<Collision> RaycastAll(const Vector3& p1, const Vector3& p2, int layerMask = Physics::MASK_ALL) override;

	void SetVelocityIterations(int iterations);
	int GetVelocityIterations() const;
	void SetPositionIterations(int iterations);
	int GetPositionIterations() const;

	b2World& GetWorld() const;
	b2Body* GetWorldBody() const;

	// Modified by ContactListener
	Array<Collision> contactsBegin;
	Array<Collision> contactsEnd;

private:
	void HandleCollisionBegins();
	void HandleCollisionEnds();

	b2World* m_pWorld = nullptr;
	b2ContactListener* m_pContactListener = nullptr;
	b2DestructionListener* m_pDestructorListener = nullptr;
	b2Body* m_pWorldBody = nullptr;

	float m_fixedUpdateTimeAccumulator = 0.0f;
	int m_velocityIterations = 8;
	int m_positionIterations = 3;
};
