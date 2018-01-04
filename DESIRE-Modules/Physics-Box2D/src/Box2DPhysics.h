#pragma once

#include "Physics/IPhysics.h"
#include "Physics/Collision.h"

class b2World;
class b2ContactListener;
class b2DestructionListener;
class b2Body;

class Box2DPhysics : public IPhysics
{
public:
	Box2DPhysics();
	~Box2DPhysics() override;

	void Update() override;

	PhysicsComponent* CreatePhysicsComponentOnObject(Object& object) override;

	bool RaycastClosest(const Vector3& p1, const Vector3& p2, PhysicsComponent **o_componentPtr, Vector3 *o_collisionPointPtr = nullptr, Vector3 *o_collisionNormalPtr = nullptr, int layerMask = IPhysics::MASK_ALL) override;
	bool RaycastAny(const Vector3& p1, const Vector3& p2, int layerMask = IPhysics::MASK_ALL) override;
	int RaycastAll(const Vector3& p1, const Vector3& p2, int maxCount, PhysicsComponent **o_components, Vector3 *o_collisionPoints = nullptr, Vector3 *o_collisionNormals = nullptr, int layerMask = IPhysics::MASK_ALL) override;

	uint16_t GetMaskForCollisionLayer(EPhysicsCollisionLayer layer) const;

	b2World* GetWorld() const;
	b2Body* GetBodyForTargetJoint() const;

	// Modified by ContactListener
	std::vector<Collision> contactsBegin;
	std::vector<Collision> contactsEnd;

private:
	void HandleCollisionBegins();
	void HandleCollisionEnds();

	b2World *world = nullptr;
	b2ContactListener *contactListener = nullptr;
	b2DestructionListener *destructorListener = nullptr;
	b2Body *bodyForTargetJoint = nullptr;
};
