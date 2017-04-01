#pragma once

#include "Component/PhysicsComponent.h"

class btRigidBody;
class btCollisionShape;
struct btDefaultMotionState;
class btTriangleIndexVertexArray;

class BulletPhysicsComponent : public PhysicsComponent
{
public:
	BulletPhysicsComponent(bool dynamic);
	~BulletPhysicsComponent();

	void SetCollisionGroup(EPhysicsCollisionGroup collisionGroup) override;
	void SetMass(float mass) override;
	float GetMass() const override;

private:
	btRigidBody *body;
	btCollisionShape *shape;
	btDefaultMotionState *motionState;
	btTriangleIndexVertexArray *triangleIndexVertexArrays;

	bool dynamic;
};
