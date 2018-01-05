#include "stdafx.h"
#include "Box2DPhysics.h"
#include "ContactListener.h"
#include "DestructorListener.h"
#include "RaycastCallbacks.h"

#include "Component/PhysicsComponent.h"
#include "Component/ScriptComponent.h"
#include "Scene/Object.h"

#include "Box2D/Dynamics/b2World.h"
#include "Box2D/Dynamics/Contacts/b2Contact.h"

static std::pair<b2Vec2, b2Vec2> GetValidRay(const Vector3& p1, const Vector3& p2)
{
	std::pair<b2Vec2, b2Vec2> ray = std::make_pair(b2Vec2(p1.GetX(), p1.GetY()), b2Vec2(p2.GetX(), p2.GetY()));

	if(ray.first.x == ray.second.x && ray.first.y == ray.second.y)
	{
		static const float kEpsilon = 0.001f;
		ray.first.x += kEpsilon;
		ray.first.y += kEpsilon;
	}

	return ray;
}

Box2DPhysics::Box2DPhysics()
{
	contactsBegin.reserve(32);
	contactsEnd.reserve(32);

	contactListener = new ContactListener();
	destructorListener = new DestructorListener();

	b2Vec2 gravity(0.0f, -10.0f);
	world = new b2World(gravity);
	world->SetContinuousPhysics(true);
	world->SetContactListener(contactListener);
	world->SetDestructionListener(destructorListener);

	b2BodyDef bodyDef;
	bodyDef.type = b2BodyType::b2_dynamicBody;
	bodyForTargetJoint = world->CreateBody(&bodyDef);
}

Box2DPhysics::~Box2DPhysics()
{
	world->DestroyBody(bodyForTargetJoint);
	bodyForTargetJoint = nullptr;

	delete world;
	world = nullptr;

	delete destructorListener;
	destructorListener = nullptr;

	delete contactListener;
	contactListener = nullptr;
}

void Box2DPhysics::Update()
{
	const float dt = 1.0f / 60.0f;

	{
		contactsBegin.clear();
		contactsEnd.clear();

		const int32 velocityIterations = 8;
		const int32 positionIterations = 3;
		world->Step(dt, velocityIterations, positionIterations);

		HandleCollisionEnds();
		HandleCollisionBegins();
	}

	world->ClearForces();
}

PhysicsComponent* Box2DPhysics::CreatePhysicsComponentOnObject(Object& object)
{
	if(object.GetComponent<PhysicsComponent>() != nullptr)
	{
		return nullptr;
	}

	ASSERT(false && "TODO");
	return nullptr;
}

bool Box2DPhysics::RaycastClosest(const Vector3& p1, const Vector3& p2, PhysicsComponent **o_componentPtr, Vector3 *o_collisionPointPtr, Vector3 *o_collisionNormalPtr, int layerMask)
{
	const std::pair<b2Vec2, b2Vec2> ray = GetValidRay(p1, p2);

	RaycastClosestCallback callback(layerMask);
	world->RayCast(&callback, ray.first, ray.second);

	if(callback.component != nullptr)
	{
		if(o_componentPtr != nullptr)
		{
			*o_componentPtr = callback.component;
		}

		if(o_collisionPointPtr != nullptr)
		{
			*o_collisionPointPtr = Vector3(callback.contactPoint.x, callback.contactPoint.y, 0.0f);
		}

		if(o_collisionNormalPtr != nullptr)
		{
			*o_collisionNormalPtr = Vector3(callback.contactNormal.x, callback.contactNormal.y, 0.0f);
		}

		return true;
	}

	return false;
}

bool Box2DPhysics::RaycastAny(const Vector3& p1, const Vector3& p2, int layerMask)
{
	const std::pair<b2Vec2, b2Vec2> ray = GetValidRay(p1, p2);

	RaycastAnyCallback callback(layerMask);
	world->RayCast(&callback, ray.first, ray.second);

	return callback.hasHit;
}

int Box2DPhysics::RaycastAll(const Vector3& p1, const Vector3& p2, int maxCount, PhysicsComponent **o_components, Vector3 *o_collisionPoints, Vector3 *o_collisionNormals, int layerMask)
{
	const std::pair<b2Vec2, b2Vec2> ray = GetValidRay(p1, p2);

	RaycastAllCallback callback(layerMask);
	world->RayCast(&callback, ray.first, ray.second);

	const int count = std::min(maxCount, (int)callback.collisions.size());
	for(int i = 0; i < count; ++i)
	{
		if(o_components != nullptr)
		{
			o_components[i] = callback.collisions[i].component;
		}

		if(o_collisionPoints != nullptr)
		{
			o_collisionPoints[i] = Vector3(callback.collisions[i].contactPoint.x, callback.collisions[i].contactPoint.x, 0.0f);
		}

		if(o_collisionNormals != nullptr)
		{
			o_collisionNormals[i] = Vector3(callback.collisions[i].contactNormal.x, callback.collisions[i].contactNormal.x, 0.0f);
		}
	}

	return count;
}

uint16_t Box2DPhysics::GetMaskForCollisionLayer(EPhysicsCollisionLayer layer) const
{
	ASSERT(layer < EPhysicsCollisionLayer::NUM);
	return (uint16_t)collisionMasks[(size_t)layer];
}

b2World* Box2DPhysics::GetWorld() const
{
	return world;
}

b2Body* Box2DPhysics::GetBodyForTargetJoint() const
{
	return bodyForTargetJoint;
}

void Box2DPhysics::HandleCollisionBegins()
{
	for(Collision& collision : contactsBegin)
	{
		if(collision.component->IsTrigger() || collision.incomingComponent->IsTrigger())
		{
			// Trigger event (only sent to triggers)
			if(collision.component->IsTrigger())
			{
				for(ScriptComponent *scriptComponent : collision.component->GetObject().GetScriptComponents())
				{
					scriptComponent->Call("OnTriggerEnter", collision.incomingComponent);
				}
			}

			if(collision.incomingComponent->IsTrigger())
			{
				for(ScriptComponent *scriptComponent : collision.incomingComponent->GetObject().GetScriptComponents())
				{
					scriptComponent->Call("OnTriggerEnter", collision.component);
				}
			}
		}
		else
		{
			// Collision event (sent to both components)
			PhysicsComponent *component0 = collision.component;
			PhysicsComponent *component1 = collision.incomingComponent;

			for(ScriptComponent *scriptComponent : component0->GetObject().GetScriptComponents())
			{
				scriptComponent->Call("OnCollisionEnter", &collision);
			}

			collision.component = component1;
			collision.incomingComponent = component0;
			for(ScriptComponent *scriptComponent : component1->GetObject().GetScriptComponents())
			{
				scriptComponent->Call("OnCollisionEnter", &collision);
			}
		}
	}
}

void Box2DPhysics::HandleCollisionEnds()
{
	for(Collision& collision : contactsEnd)
	{
		if(collision.component->IsTrigger() || collision.incomingComponent->IsTrigger())
		{
			// Trigger event (only sent to triggers)
			if(collision.component->IsTrigger())
			{
				for(ScriptComponent *scriptComponent : collision.component->GetObject().GetScriptComponents())
				{
					scriptComponent->Call("OnTriggerExit", collision.incomingComponent);
				}
			}

			if(collision.incomingComponent->IsTrigger())
			{
				for(ScriptComponent *scriptComponent : collision.incomingComponent->GetObject().GetScriptComponents())
				{
					scriptComponent->Call("OnTriggerExit", collision.component);
				}
			}
		}
		else
		{
			// Collision event (sent to both components)
			PhysicsComponent *component0 = collision.component;
			PhysicsComponent *component1 = collision.incomingComponent;

			for(ScriptComponent *scriptComponent : component0->GetObject().GetScriptComponents())
			{
				scriptComponent->Call("OnCollisionExit", &collision);
			}

			collision.component = component1;
			collision.incomingComponent = component0;
			for(ScriptComponent *scriptComponent : component1->GetObject().GetScriptComponents())
			{
				scriptComponent->Call("OnCollisionExit", &collision);
			}
		}
	}
}
