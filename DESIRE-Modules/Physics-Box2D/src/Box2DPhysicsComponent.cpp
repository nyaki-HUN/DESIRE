#include "Box2DPhysicsComponent.h"
#include "Box2DPhysics.h"

#include "Engine/Core/Modules.h"

#include "Box2D/Dynamics/b2World.h"

Box2DPhysicsComponent::Box2DPhysicsComponent(Object& object)
	: PhysicsComponent(object)
{
	b2World *world = static_cast<Box2DPhysics*>(Modules::Physics.get())->GetWorld();

	b2BodyDef bodyDef;
	bodyDef.userData = this;
	body = world->CreateBody(&bodyDef);
}

Box2DPhysicsComponent::~Box2DPhysicsComponent()
{
	ReleaseFixtures();

	b2World *world = static_cast<Box2DPhysics*>(Modules::Physics.get())->GetWorld();
	world->DestroyBody(body);
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

}

float Box2DPhysicsComponent::GetMass() const
{
	return body->GetMass();
}

bool Box2DPhysicsComponent::IsTrigger() const
{
	return isTrigger;
}

b2Body* Box2DPhysicsComponent::GetBody() const
{
	return body;
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
	Box2DPhysics *physics = static_cast<Box2DPhysics*>(Modules::Physics.get());

	filterData.categoryBits = 1 << (int)collisionLayer;
	filterData.maskBits = physics->GetMaskForCollisionLayer(collisionLayer);

	for(b2Fixture *fixture : fixtures)
	{
		fixture->SetFilterData(filterData);
	}
}
