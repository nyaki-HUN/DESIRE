#pragma once

#include "Engine/Component/Component.h"
#include "Engine/Core/math/Vector3.h"
#include "Engine/Physics/PhysicsMaterial.h"

#include <memory>
#include <vector>

enum class EPhysicsCollisionLayer;
class ColliderShape;

class PhysicsComponent : public Component
{
	DECLARE_COMPONENT_FURCC_TYPE_ID("PHYS");

protected:
	PhysicsComponent(Object& object);

public:
	~PhysicsComponent() override;

	virtual void SetCollisionLayer(EPhysicsCollisionLayer collisionLayer);
	EPhysicsCollisionLayer GetCollisionLayer() const;

	const PhysicsMaterial& GetPhysicsMaterial() const;

	virtual std::vector<PhysicsComponent*> GetActiveCollidingComponents() const = 0;

	virtual void SetMass(float mass) = 0;
	virtual float GetMass() const = 0;

	virtual Vector3 GetCenterOfMass() const = 0;

	virtual void SetTrigger(bool value) = 0;
	virtual bool IsTrigger() const = 0;

	virtual void SetLinearDamping(float value) = 0;
	virtual float GetLinearDamping() const = 0;
	virtual void SetAngularDamping(float value) = 0;
	virtual float GetAngularDamping() const = 0;

protected:
	EPhysicsCollisionLayer collisionLayer;
	PhysicsMaterial physicsMaterial;
	std::unique_ptr<ColliderShape> shape;
};
