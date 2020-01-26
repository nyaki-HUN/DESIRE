#pragma once

#include "box2d/b2_world_callbacks.h"

class Collision;

class ContactListener : public b2ContactListener
{
public:
	void BeginContact(b2Contact* contact) override;
	void EndContact(b2Contact* contact) override;

	void PreSolve(b2Contact* contact, const b2Manifold* oldManifold) override;
	void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) override;

private:
	static void FillCollisionFromContact(Collision& collision, const b2Contact* contact);
};
