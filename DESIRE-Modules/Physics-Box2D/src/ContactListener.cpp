#include "stdafx.h"
#include "ContactListener.h"
#include "Box2DPhysics.h"

#include "Box2D/Dynamics/Contacts/b2Contact.h"

ContactListener::ContactListener()
{

}

ContactListener::~ContactListener()
{

}

void ContactListener::BeginContact(b2Contact *contact)
{
	std::vector<Collision>& contacts = static_cast<Box2DPhysics*>(Physics::Get())->contactsBegin;
	contacts.emplace_back();
	FillCollisionFromContact(contacts.back(), contact);
}

void ContactListener::EndContact(b2Contact *contact)
{
	std::vector<Collision>& contacts = static_cast<Box2DPhysics*>(Physics::Get())->contactsEnd;
	contacts.emplace_back();
	FillCollisionFromContact(contacts.back(), contact);
}

void ContactListener::PreSolve(b2Contact *contact, const b2Manifold *oldManifold)
{

}

void ContactListener::PostSolve(b2Contact *contact, const b2ContactImpulse *impulse)
{

}

void ContactListener::FillCollisionFromContact(Collision& collision, const b2Contact *contact)
{
	const b2Fixture *fixtureA = contact->GetFixtureA();
	const b2Fixture *fixtureB = contact->GetFixtureB();

	collision.component = static_cast<PhysicsComponent*>(fixtureA->GetUserData());
	collision.incomingComponent = static_cast<PhysicsComponent*>(fixtureB->GetUserData());

	b2WorldManifold worldManifold;
	contact->GetWorldManifold(&worldManifold);

	collision.pointCount = std::min(contact->GetManifold()->pointCount, Collision::MAX_CONTACT_POINTS);
	collision.contactPoints[0] = Vector3(worldManifold.points[0].x, worldManifold.points[0].y, 0.0f);
	collision.contactPoints[1] = Vector3(worldManifold.points[1].x, worldManifold.points[1].y, 0.0f);
}
