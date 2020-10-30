#include "stdafx_Box2D.h"
#include "Box2DPhysicsComponent.h"

#include "b2MathExt.h"
#include "Box2DColliderShape.h"
#include "Box2DPhysics.h"

#include "Engine/Core/Math/Transform.h"
#include "Engine/Core/Object.h"

Box2DPhysicsComponent::Box2DPhysicsComponent(Object& object)
	: PhysicsComponent(object)
{
	b2World& world = static_cast<Box2DPhysics&>(*Modules::Physics).GetWorld();

	b2BodyDef bodyDef;
	bodyDef.angularDamping = 0.05f;
	bodyDef.userData = this;
	m_pBody = world.CreateBody(&bodyDef);

	m_filterData.categoryBits = 1 << static_cast<int>(m_collisionLayer);
	m_filterData.maskBits = Modules::Physics->GetMaskForCollisionLayer(m_collisionLayer);
}

Box2DPhysicsComponent::~Box2DPhysicsComponent()
{
	ReleaseFixtures();

	m_pBody->GetWorld()->DestroyBody(m_pBody);
	m_pBody = nullptr;
}

void Box2DPhysicsComponent::SetEnabled(bool value)
{
	if(IsEnabled() == value)
	{
		return;
	}

	PhysicsComponent::SetEnabled(value);
	 m_pBody->SetActive(value);
}

void Box2DPhysicsComponent::CloneTo(Object& otherObject) const
{
	PhysicsComponent::CloneTo(otherObject);
	Box2DPhysicsComponent* otherComponent = otherObject.GetComponent<Box2DPhysicsComponent>();
	otherComponent->SetDensity(m_density);
}

void Box2DPhysicsComponent::SetCollisionLayer(EPhysicsCollisionLayer collisionLayer)
{
	m_collisionLayer = collisionLayer;

	m_filterData.categoryBits = 1 << static_cast<int>(collisionLayer);
	m_filterData.maskBits = Modules::Physics->GetMaskForCollisionLayer(collisionLayer);

	for(b2Fixture* pFixture : m_fixtures)
	{
		pFixture->SetFilterData(m_filterData);
	}
}

void Box2DPhysicsComponent::SetCollisionDetectionMode(ECollisionDetectionMode mode)
{
	 m_pBody->SetBullet(mode == ECollisionDetectionMode::Continuous);
}

PhysicsComponent::ECollisionDetectionMode Box2DPhysicsComponent::GetCollisionDetectionMode() const
{
	return  m_pBody->IsBullet() ? ECollisionDetectionMode::Continuous : ECollisionDetectionMode::Discrete;
}

Array<PhysicsComponent*> Box2DPhysicsComponent::GetActiveCollidingComponents() const
{
	Array<PhysicsComponent*> collisions;

	int count = 0;
	b2ContactEdge* pContactEdge =  m_pBody->GetContactList();
	while(pContactEdge != nullptr)
	{
		const b2Contact* pContact = pContactEdge->contact;
		if(pContact->IsTouching())
		{
			const b2Fixture* pFixtureA = pContact->GetFixtureA();
			const b2Fixture* pFixtureB = pContact->GetFixtureB();
			Box2DPhysicsComponent* pComponentA = static_cast<Box2DPhysicsComponent*>(pFixtureA->GetUserData());
			Box2DPhysicsComponent* pComponentB = static_cast<Box2DPhysicsComponent*>(pFixtureB->GetUserData());
			collisions.Add((this == pComponentA) ? pComponentB : pComponentA);
		}

		pContactEdge = pContactEdge->next;
	}

	return collisions;
}

void Box2DPhysicsComponent::SetBodyType(EBodyType bodyType)
{
	switch(bodyType)
	{
		case EBodyType::Static:		 m_pBody->SetType(b2BodyType::b2_staticBody); break;
		case EBodyType::Dynamic:	 m_pBody->SetType(b2BodyType::b2_dynamicBody); break;
		case EBodyType::Kinematic:	 m_pBody->SetType(b2BodyType::b2_kinematicBody); break;
	}
}

PhysicsComponent::EBodyType Box2DPhysicsComponent::GetBodyType() const
{
	switch(m_pBody->GetType())
	{
		case b2BodyType::b2_staticBody:		return EBodyType::Static;
		case b2BodyType::b2_kinematicBody:	return EBodyType::Kinematic;
		case b2BodyType::b2_dynamicBody:	return EBodyType::Dynamic;
	}

	return EBodyType::Static;
}

void Box2DPhysicsComponent::SetTrigger(bool value)
{
	m_isTrigger = value;

	for(b2Fixture* pFixture : m_fixtures)
	{
		pFixture->SetSensor(m_isTrigger);
	}

	// Need to refresh density when isTrigger is changed
	SetDensity(m_density);
}

bool Box2DPhysicsComponent::IsTrigger() const
{
	return m_isTrigger;
}

void Box2DPhysicsComponent::SetMass(float mass)
{
	DESIRE_TODO("Implement SetMass()");
}

float Box2DPhysicsComponent::GetMass() const
{
	return  m_pBody->GetMass();
}

Vector3 Box2DPhysicsComponent::GetCenterOfMass() const
{
	const b2Vec2& vec =  m_pBody->GetWorldCenter();
	return Vector3(vec.x, vec.y, 0.0f);
}

void Box2DPhysicsComponent::SetDensity(float density)
{
	m_density = density;

	const float densityToSet = GetDensity();
	for(b2Fixture* pFixture : m_fixtures)
	{
		pFixture->SetDensity(densityToSet);
	}

	// The SetDensity() didn't adjust the mass, so we need to do it
	 m_pBody->ResetMassData();
}

float Box2DPhysicsComponent::GetDensity() const
{
	// Triggers are required to have zero density
	return (m_isTrigger ? 0.0f : m_density);
}

void Box2DPhysicsComponent::SetLinearDamping(float value)
{
	 m_pBody->SetLinearDamping(value);
}

float Box2DPhysicsComponent::GetLinearDamping() const
{
	return  m_pBody->GetLinearDamping();
}

void Box2DPhysicsComponent::SetAngularDamping(float value)
{
	 m_pBody->SetAngularDamping(value);
}

float Box2DPhysicsComponent::GetAngularDamping() const
{
	return  m_pBody->GetAngularDamping();
}

void Box2DPhysicsComponent::SetLinearVelocity(const Vector3& linearVelocity)
{
	 m_pBody->SetLinearVelocity(GetB2Vec2(linearVelocity));
}

Vector3 Box2DPhysicsComponent::GetLinearVelocity() const
{
	return GetVector3( m_pBody->GetLinearVelocity());
}

void Box2DPhysicsComponent::SetAngularVelocity(const Vector3& angularVelocity)
{
	 m_pBody->SetAngularVelocity(angularVelocity.GetZ());
}

Vector3 Box2DPhysicsComponent::GetAngularVelocity() const
{
	const float angularVelocity =  m_pBody->GetAngularVelocity();
	return Vector3(0.0f, 0.0f, angularVelocity);
}

void Box2DPhysicsComponent::AddForce(const Vector3& force, EForceMode mode)
{
	const b2Vec2 f = GetB2Vec2(force);
	switch(mode)
	{
		case EForceMode::Force:		 m_pBody->ApplyForceToCenter(f, true); break;
		case EForceMode::Impulse:	 m_pBody->ApplyLinearImpulseToCenter(f, true); break;
	}
}

void Box2DPhysicsComponent::AddForceAtPosition(const Vector3& force, const Vector3& position, EForceMode mode)
{
	const b2Vec2 f = GetB2Vec2(force);
	const b2Vec2 pos = GetB2Vec2(position);
	switch(mode)
	{
		case EForceMode::Force:		 m_pBody->ApplyForce(f, pos, true); break;
		case EForceMode::Impulse:	 m_pBody->ApplyLinearImpulse(f, pos, true); break;
	}
}

void Box2DPhysicsComponent::AddTorque(const Vector3& torque, EForceMode mode)
{
	switch(mode)
	{
		case EForceMode::Force:		 m_pBody->ApplyTorque(torque.GetZ(), true); break;
		case EForceMode::Impulse:	 m_pBody->ApplyAngularImpulse(torque.GetZ(), true); break;
	}
}

void Box2DPhysicsComponent::SetLinearMotionLock(bool axisX, bool axisY, bool axisZ)
{
}

void Box2DPhysicsComponent::SetAngularMotionLock(bool axisX, bool axisY, bool axisZ)
{
	 m_pBody->SetFixedRotation(axisZ);
}

bool Box2DPhysicsComponent::IsSleeping() const
{
	return ( m_pBody->IsAwake() == false);
}

void Box2DPhysicsComponent::UpdateGameObjectTransform() const
{
	Transform& transform = object.GetTransform();
	const b2Vec2& pos =  m_pBody->GetPosition();
	const float rotAngle =  m_pBody->GetTransform().q.GetAngle();
	transform.SetPosition(Vector3(pos.x, pos.y, transform.GetPosition().GetZ()));
	transform.SetRotation(Quat::CreateRotationFromEulerAngles(Vector3(0.0f, 0.0f, rotAngle)));
}

void Box2DPhysicsComponent::SetTransformFromGameObject()
{
	const Transform& transform = object.GetTransform();
	const Vector3 pos = transform.GetPosition();
	const float rotAngle = transform.GetRotation().EulerAngles().GetZ();
	 m_pBody->SetTransform(GetB2Vec2(pos), rotAngle);
}

b2Body* Box2DPhysicsComponent::GetBody() const
{
	return m_pBody;
}

void Box2DPhysicsComponent::CreateFixtures()
{
	ReleaseFixtures();

	if(m_spShape == nullptr)
	{
		return;
	}

	Box2DColliderShape& colliderShape = static_cast<Box2DColliderShape&>(*m_spShape);
	for(size_t i = 0; i < colliderShape.GetShapeCount(); ++i)
	{
		b2FixtureDef fixtureDef;
		fixtureDef.shape = colliderShape.GetShape(i);
		fixtureDef.userData = this;
		fixtureDef.friction = m_physicsMaterial.friction;
		fixtureDef.restitution = m_physicsMaterial.bounciness;
		fixtureDef.density = GetDensity();
		fixtureDef.isSensor = m_isTrigger;
		fixtureDef.filter = m_filterData;

		b2Fixture* pFixture =  m_pBody->CreateFixture(&fixtureDef);
		m_fixtures.Add(pFixture);
	}
}

void Box2DPhysicsComponent::ReleaseFixtures()
{
	for(b2Fixture* pFixture : m_fixtures)
	{
		 m_pBody->DestroyFixture(pFixture);
	}

	m_fixtures.Clear();
}
