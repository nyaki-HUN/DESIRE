#include "stdafx_Box2D.h"
#include "ContactListener.h"

#include "Box2DPhysics.h"

void ContactListener::BeginContact(b2Contact* pContact)
{
	Array<Collision>& pContacts = static_cast<Box2DPhysics*>(Modules::Physics.get())->contactsBegin;
	Collision& collision = pContacts.EmplaceAdd();
	FillCollisionFromContact(collision, pContact);
}

void ContactListener::EndContact(b2Contact* pContact)
{
	Array<Collision>& pContacts = static_cast<Box2DPhysics*>(Modules::Physics.get())->contactsEnd;
	Collision& collision = pContacts.EmplaceAdd();
	FillCollisionFromContact(collision, pContact);
}

void ContactListener::PreSolve(b2Contact* pContact, const b2Manifold* oldManifold)
{
}

void ContactListener::PostSolve(b2Contact* pContact, const b2ContactImpulse* impulse)
{
}

void ContactListener::FillCollisionFromContact(Collision& collision, const b2Contact* pContact)
{
	const b2Fixture* pFixtureA = pContact->GetFixtureA();
	const b2Fixture* pFixtureB = pContact->GetFixtureB();

	collision.pComponent = static_cast<PhysicsComponent*>(pFixtureA->GetUserData());
	collision.pIncomingComponent = static_cast<PhysicsComponent*>(pFixtureB->GetUserData());

	b2WorldManifold worldManifold;
	pContact->GetWorldManifold(&worldManifold);

	collision.pointCount = std::min(pContact->GetManifold()->pointCount, Collision::kMaxContactPoints);
	for(int i = 0; i < collision.pointCount; ++i)
	{
		collision.contactPoints[i] = Vector3(worldManifold.points[i].x, worldManifold.points[i].y, 0.0f);
		collision.contactNormals[i] = Vector3(worldManifold.normal.x, worldManifold.normal.y, 0.0f);
	}
}
