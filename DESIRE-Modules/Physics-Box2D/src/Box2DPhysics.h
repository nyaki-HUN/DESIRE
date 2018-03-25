#pragma once

#include "Engine/Physics/Physics.h"

#include <vector>

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
	std::vector<Collision> RaycastAll(const Vector3& p1, const Vector3& p2, int layerMask = Physics::MASK_ALL) override;

	uint16_t GetMaskForCollisionLayer(EPhysicsCollisionLayer layer) const;

	b2World* GetWorld() const;
	b2Body* GetWorldBody() const;

	// Modified by ContactListener
	std::vector<Collision> contactsBegin;
	std::vector<Collision> contactsEnd;

private:
	void HandleCollisionBegins();
	void HandleCollisionEnds();

	b2World *world = nullptr;
	b2ContactListener *contactListener = nullptr;
	b2DestructionListener *destructorListener = nullptr;
	b2Body *worldBody = nullptr;
};
