#include "stdafx.h"
#include "Box2DPhysicsComponent.h"
#include "Box2DPhysics.h"

#include "Box2D/Dynamics/b2World.h"

Box2DPhysicsComponent::Box2DPhysicsComponent(Object& object)
	: PhysicsComponent(object)
{
	Box2DPhysics *physics = static_cast<Box2DPhysics*>(IPhysics::Get());

	b2BodyDef bodyDef;
	bodyDef.userData = this;
	body = physics->GetWorld()->CreateBody(&bodyDef);
}

Box2DPhysicsComponent::~Box2DPhysicsComponent()
{
	Box2DPhysics *physics = static_cast<Box2DPhysics*>(IPhysics::Get());

	ReleaseFixtures();

	physics->GetWorld()->DestroyBody(body);
	body = nullptr;
}

void Box2DPhysicsComponent::SetCollisionLayer(EPhysicsCollisionLayer i_collisionLayer)
{
	if(collisionLayer == i_collisionLayer)
	{
		return;
	}

	collisionLayer = i_collisionLayer;
	UpdateFilterData();
}

void Box2DPhysicsComponent::SetMass(float mass)
{
	ASSERT(false && "TODO");
}

float Box2DPhysicsComponent::GetMass() const
{
	ASSERT(false && "TODO");
	return 0.0f;
}

bool Box2DPhysicsComponent::IsTrigger() const
{
	return isTrigger;
}

void Box2DPhysicsComponent::ReleaseFixtures()
{
	for(b2Fixture *fixture : fixtures)
	{
		body->DestroyFixture(fixture);
	}

	fixtures.clear();
}

void Box2DPhysicsComponent::UpdateFilterData()
{
	Box2DPhysics *physics = static_cast<Box2DPhysics*>(IPhysics::Get());

	filterData.categoryBits = 1 << (int)collisionLayer;
	filterData.maskBits = physics->GetMaskForCollisionLayer(collisionLayer);

	for(b2Fixture *fixture : fixtures)
	{
		fixture->SetFilterData(filterData);
	}
}
