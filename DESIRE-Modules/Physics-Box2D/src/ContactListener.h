#pragma once

#include "box2d/b2_world_callbacks.h"

struct Collision;

class ContactListener : public b2ContactListener
{
public:
	void BeginContact(b2Contact* pContact) override;
	void EndContact(b2Contact* pContact) override;

	void PreSolve(b2Contact* pContact, const b2Manifold* pOldManifold) override;
	void PostSolve(b2Contact* pContact, const b2ContactImpulse* pImpulse) override;

private:
	static void FillCollisionFromContact(Collision& collision, const b2Contact* pContact);
};
