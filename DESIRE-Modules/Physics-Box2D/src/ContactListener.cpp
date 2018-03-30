#include "ContactListener.h"
#include "Box2DPhysics.h"

#include "Engine/Core/Modules.h"

#include "Box2D/Dynamics/Contacts/b2Contact.h"

#include <algorithm>		// for std::min

ContactListener::ContactListener()
{

}

ContactListener::~ContactListener()
{

}

void ContactListener::BeginContact(b2Contact *contact)
{
	std::vector<Collision>& contacts = static_cast<Box2DPhysics*>(Modules::Physics.get())->contactsBegin;
	contacts.emplace_back();
	FillCollisionFromContact(contacts.back(), contact);
}

void ContactListener::EndContact(b2Contact *contact)
{
	std::vector<Collision>& contacts = static_cast<Box2DPhysics*>(Modules::Physics.get())->contactsEnd;
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

	collision.pointCount = std::min(contact->GetManifold()->pointCount, Collision::k_MaxContactPoints);
	for(int i = 0; i < collision.pointCount; ++i)
	{
		collision.contactPoints[i] = Vector3(worldManifold.points[i].x, worldManifold.points[i].y, 0.0f);
		collision.contactNormals[i] = Vector3::Zero();
	}
}
