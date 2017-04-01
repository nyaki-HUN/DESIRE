#pragma once

#include "Physics/IPhysics.h"

class PhysXPhysics : public IPhysics
{
public:
	PhysXPhysics();
	~PhysXPhysics();

	void Update() override;

	PhysicsComponent* CreatePhysicsComponent() override;

	bool RayTest(const Vector3& startPoint, const Vector3& direction, Vector3 *o_hitpoint = nullptr, PhysicsComponent **o_component = nullptr, int collisionGroupMask = 0xffffffff) override;
};
