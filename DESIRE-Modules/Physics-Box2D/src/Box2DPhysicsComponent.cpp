#include "Box2DPhysicsComponent.h"
#include "Box2DColliderShape.h"
#include "Box2DPhysics.h"
#include "b2MathExt.h"

#include "Engine/Modules.h"
#include "Engine/Scene/Object.h"
#include "Engine/Scene/Transform.h"

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

	filterData.categoryBits = 1 << (int)collisionLayer;
	filterData.maskBits = Modules::Physics->GetMaskForCollisionLayer(collisionLayer);
}

Box2DPhysicsComponent::~Box2DPhysicsComponent()
{
	ReleaseFixtures();

	b2World *world = static_cast<Box2DPhysics*>(Modules::Physics.get())->GetWorld();
	world->DestroyBody(body);
	body = nullptr;
}

void Box2DPhysicsComponent::SetEnabled(bool value)
{
	if(IsEnabled() == value)
	{
		return;
	}

	PhysicsComponent::SetEnabled(value);
	body->SetActive(value);
}

void Box2DPhysicsComponent::CloneTo(Object& otherObject) const
{
	PhysicsComponent::CloneTo(otherObject);
	Box2DPhysicsComponent *otherComponent = otherObject.GetComponent<Box2DPhysicsComponent>();
	otherComponent->SetDensity(density);
}

void Box2DPhysicsComponent::SetCollisionLayer(EPhysicsCollisionLayer i_collisionLayer)
{
	collisionLayer = i_collisionLayer;

	filterData.categoryBits = 1 << (int)collisionLayer;
	filterData.maskBits = Modules::Physics->GetMaskForCollisionLayer(collisionLayer);

	for(b2Fixture *fixture : fixtures)
	{
		fixture->SetFilterData(filterData);
	}
}

void Box2DPhysicsComponent::SetCollisionDetectionMode(ECollisionDetectionMode mode)
{
	body->SetBullet(mode == ECollisionDetectionMode::Continuous);
}

PhysicsComponent::ECollisionDetectionMode Box2DPhysicsComponent::GetCollisionDetectionMode() const
{
	return body->IsBullet() ? ECollisionDetectionMode::Continuous : ECollisionDetectionMode::Discrete;
}

Array<PhysicsComponent*> Box2DPhysicsComponent::GetActiveCollidingComponents() const
{
	Array<PhysicsComponent*> collisions;

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
			collisions.Add((this == componentA) ? componentB : componentA);
		}

		contactEdge = contactEdge->next;
	}

	return collisions;
}

void Box2DPhysicsComponent::SetBodyType(EBodyType bodyType)
{
	switch(bodyType)
	{
		case EBodyType::Static:		body->SetType(b2BodyType::b2_staticBody); break;
		case EBodyType::Dynamic:	body->SetType(b2BodyType::b2_dynamicBody); break;
		case EBodyType::Kinematic:	body->SetType(b2BodyType::b2_kinematicBody); break;
	}
}

PhysicsComponent::EBodyType Box2DPhysicsComponent::GetBodyType() const
{
	switch(body->GetType())
	{
		case b2BodyType::b2_staticBody:		return EBodyType::Static;
		case b2BodyType::b2_kinematicBody:	return EBodyType::Kinematic;
		case b2BodyType::b2_dynamicBody:	return EBodyType::Dynamic;
	}

	return EBodyType::Static;
}

void Box2DPhysicsComponent::SetTrigger(bool value)
{
	isTrigger = value;

	for(b2Fixture *fixture : fixtures)
	{
		fixture->SetSensor(isTrigger);
	}

	// Need to refresh density when isTrigger is changed
	SetDensity(density);
}

bool Box2DPhysicsComponent::IsTrigger() const
{
	return isTrigger;
}

void Box2DPhysicsComponent::SetMass(float mass)
{
	DESIRE_TODO("Implement SetMass()");
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

	const float densityToSet = GetDensity();
	for(b2Fixture *fixture : fixtures)
	{
		fixture->SetDensity(densityToSet);
	}

	// The SetDensity() didn't adjust the mass, so we need to do it
	body->ResetMassData();
}

float Box2DPhysicsComponent::GetDensity() const
{
	// Triggers are required to have zero density
	return (isTrigger ? 0.0f : density);
}

void Box2DPhysicsComponent::SetLinearDamping(float value)
{
	body->SetLinearDamping(value);
}

float Box2DPhysicsComponent::GetLinearDamping() const
{
	return body->GetLinearDamping();
}

void Box2DPhysicsComponent::SetAngularDamping(float value)
{
	body->SetAngularDamping(value);
}

float Box2DPhysicsComponent::GetAngularDamping() const
{
	return body->GetAngularDamping();
}

void Box2DPhysicsComponent::SetLinearVelocity(const Vector3& linearVelocity)
{
	body->SetLinearVelocity(GetB2Vec2(linearVelocity));
}

Vector3 Box2DPhysicsComponent::GetLinearVelocity() const
{
	return GetVector3(body->GetLinearVelocity());
}

void Box2DPhysicsComponent::SetAngularVelocity(const Vector3& angularVelocity)
{
	body->SetAngularVelocity(angularVelocity.GetZ());
}

Vector3 Box2DPhysicsComponent::GetAngularVelocity() const
{
	const float angularVelocity = body->GetAngularVelocity();
	return Vector3(0.0f, 0.0f, angularVelocity);
}

void Box2DPhysicsComponent::AddForce(const Vector3& force, EForceMode mode)
{
	const b2Vec2 f = GetB2Vec2(force);
	switch(mode)
	{
		case EForceMode::Force:		body->ApplyForceToCenter(f, true); break;
		case EForceMode::Impulse:	body->ApplyLinearImpulseToCenter(f, true); break;
	}
}

void Box2DPhysicsComponent::AddForceAtPosition(const Vector3& force, const Vector3& position, EForceMode mode)
{
	const b2Vec2 f = GetB2Vec2(force);
	const b2Vec2 pos = GetB2Vec2(position);
	switch(mode)
	{
		case EForceMode::Force:		body->ApplyForce(f, pos, true); break;
		case EForceMode::Impulse:	body->ApplyLinearImpulse(f, pos, true); break;
	}
}

void Box2DPhysicsComponent::AddTorque(const Vector3& torque, EForceMode mode)
{
	switch(mode)
	{
		case EForceMode::Force:		body->ApplyTorque(torque.GetZ(), true); break;
		case EForceMode::Impulse:	body->ApplyAngularImpulse(torque.GetZ(), true); break;
	}
}

void Box2DPhysicsComponent::SetLinearMotionLock(bool axisX, bool axisY, bool axisZ)
{

}

void Box2DPhysicsComponent::SetAngularMotionLock(bool axisX, bool axisY, bool axisZ)
{
	body->SetFixedRotation(axisZ);
}

bool Box2DPhysicsComponent::IsSleeping() const
{
	return (body->IsAwake() == false);
}

void Box2DPhysicsComponent::UpdateGameObjectTransform() const
{
	Transform& transform = object.GetTransform();
	const b2Vec2& pos = body->GetPosition();
	const float rotAngle = body->GetTransform().q.GetAngle();
	transform.SetPosition(Vector3(pos.x, pos.y, transform.GetPosition().GetZ()));
	transform.SetRotation(Quat::CreateRotationFromEulerAngles(Vector3(0.0f, 0.0f, rotAngle)));
}

void Box2DPhysicsComponent::SetTransformFromGameObject()
{
	const Transform& transform = object.GetTransform();
	const Vector3 pos = transform.GetPosition();
	const float rotAngle = transform.GetRotation().EulerAngles().GetZ();
	body->SetTransform(GetB2Vec2(pos), rotAngle);
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
		fixtureDef.density = GetDensity();
		fixtureDef.isSensor = isTrigger;
		fixtureDef.filter = filterData;

		b2Fixture *fixture = body->CreateFixture(&fixtureDef);
		fixtures.Add(fixture);
	}
}

void Box2DPhysicsComponent::ReleaseFixtures()
{
	for(b2Fixture *fixture : fixtures)
	{
		body->DestroyFixture(fixture);
	}

	fixtures.Clear();
}
