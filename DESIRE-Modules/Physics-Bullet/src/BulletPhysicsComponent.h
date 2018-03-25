#pragma once

#include "Engine/Component/PhysicsComponent.h"

class btRigidBody;
class btCollisionShape;
struct btDefaultMotionState;
class btTriangleIndexVertexArray;

class BulletPhysicsComponent : public PhysicsComponent
{
public:
	BulletPhysicsComponent(Object& object, bool dynamic);
	~BulletPhysicsComponent();

	void SetCollisionLayer(EPhysicsCollisionLayer collisionLayer) override;

	void SetMass(float mass) override;
	float GetMass() const override;

	Vector3 GetCenterOfMass() const override;

	void SetTrigger(bool value) override;
	bool IsTrigger() const override;

private:
	btRigidBody *body = nullptr;
	btCollisionShape *shape = nullptr;
	btDefaultMotionState *motionState = nullptr;
	btTriangleIndexVertexArray *triangleIndexVertexArrays = nullptr;

	bool dynamic = false;
};
