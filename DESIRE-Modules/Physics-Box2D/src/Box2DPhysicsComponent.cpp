#include "Box2DPhysicsComponent.h"
#include "Box2DColliderShape.h"
#include "Box2DPhysics.h"
#include "b2MathExt.h"

#include "Engine/Core/Modules.h"

#include "Box2D/Dynamics/b2World.h"
#include "Box2D/Dynamics/Contacts/b2Contact.h"

Box2DPhysicsComponent::Box2DPhysicsComponent(Object& object)
	: PhysicsComponent(object)
{
	b2World *world = static_cast<Box2DPhysics*>(Modules::Physics.get())->GetWorld();

	b2BodyDef bodyDef;
	bodyDef.angularDamping = 0.05f;
	bodyDef.userData = this;
	body = world->CreateBody(&bodyDef);

	UpdateFilterData();
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

std::vector<PhysicsComponent*> Box2DPhysicsComponent::GetActiveCollidingComponents() const
{
	std::vector<PhysicsComponent*> collisions;

	int count = 0;
	b2ContactEdge *contactEdge = body->GetContactList();
	while(contactEdge != nullptr)
	{
		const b2Contact *contact = contactEdge->contact;
		if(contact->IsTouching())
		{
			const b2Fixture *fixtureA = contact->GetFixtureA();
			const b2Fixture *fixtureB = contact->GetFixtureB();
			Box2DPhysicsComponent *componentA = static_cast<Box2DPhysicsComponent*>(fixtureA->GetUserData());
			Box2DPhysicsComponent *componentB = static_cast<Box2DPhysicsComponent*>(fixtureB->GetUserData());
			collisions.push_back((this == componentA) ? componentB : componentA);
		}

		contactEdge = contactEdge->next;
	}

	return collisions;
}

void Box2DPhysicsComponent::SetMass(float mass)
{

}

float Box2DPhysicsComponent::GetMass() const
{
	return body->GetMass();
}

Vector3 Box2DPhysicsComponent::GetCenterOfMass() const
{
	const b2Vec2& vec = body->GetWorldCenter();
	return Vector3(vec.x, vec.y, 0.0f);
}

void Box2DPhysicsComponent::SetDensity(float i_density)
{
	density = i_density;

	for(b2Fixture *fixture : fixtures)
	{
		fixture->SetDensity(density);
	}

	// The SetDensity() didn't adjust the mass, so we need to do it
	body->ResetMassData();
}

float Box2DPhysicsComponent::GetDensity() const
{
	return density;
}

void Box2DPhysicsComponent::SetTrigger(bool value)
{
	isTrigger = value;

	for(b2Fixture *fixture : fixtures)
	{
		fixture->SetSensor(isTrigger);
	}
}

bool Box2DPhysicsComponent::IsTrigger() const
{
	return isTrigger;
}

float Box2DPhysicsComponent::GetLinearDrag() const
{
	return body->GetLinearDamping();
}

void Box2DPhysicsComponent::SetLinearDrag(float value)
{
	body->SetLinearDamping(value);
}

float Box2DPhysicsComponent::GetAngularDrag() const
{
	return body->GetAngularDamping();
}

void Box2DPhysicsComponent::SetAngularDrag(float value)
{
	body->SetAngularDamping(value);
}

Vector2 Box2DPhysicsComponent::GetLinearVelocity()
{
	const b2Vec2& v = body->GetLinearVelocity();
	return GetVector2(v);
}

void Box2DPhysicsComponent::SetLinearVelocity(const Vector2& velocity)
{
	body->SetLinearVelocity(GetB2Vec2(velocity));
}

float Box2DPhysicsComponent::GetAngularVelocity() const
{
	return body->GetAngularVelocity();
}

void Box2DPhysicsComponent::SetAngularVelocity(float value)
{
	body->SetAngularVelocity(value);
}

bool Box2DPhysicsComponent::IsAwake() const
{
	return body->IsAwake();
}

bool Box2DPhysicsComponent::IsSleeping() const
{
	return body->IsAwake() == false;
}

void Box2DPhysicsComponent::SetEnabled(bool value)
{
	PhysicsComponent::SetEnabled(value);
	body->SetActive(value);
}

b2Body* Box2DPhysicsComponent::GetBody() const
{
	return body;
}

void Box2DPhysicsComponent::CreateFixtures()
{
	ReleaseFixtures();

	if(shape == nullptr)
	{
		return;
	}

	Box2DColliderShape *colliderShape = static_cast<Box2DColliderShape*>(shape.get());
	for(size_t i = 0; i < colliderShape->GetShapeCount(); ++i)
	{
		b2FixtureDef fixtureDef;
		fixtureDef.shape = colliderShape->GetShape(i);
		fixtureDef.userData = this;
		fixtureDef.friction = physicsMaterial.friction;
		fixtureDef.restitution = physicsMaterial.bounciness;
		fixtureDef.density = density;
		fixtureDef.isSensor = isTrigger;
		fixtureDef.filter = filterData;

		b2Fixture *fixture = body->CreateFixture(&fixtureDef);
		fixtures.push_back(fixture);
	}
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
