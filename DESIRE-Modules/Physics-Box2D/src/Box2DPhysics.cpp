#include "Box2DPhysics.h"
#include "Box2DPhysicsComponent.h"
#include "ContactListener.h"
#include "DestructorListener.h"
#include "RaycastCallbacks.h"

#include "Engine/Core/assert.h"
#include "Engine/Core/Object.h"
#include "Engine/Physics/PhysicsComponent.h"
#include "Engine/Script/ScriptComponent.h"

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
	contactsBegin.Reserve(32);
	contactsEnd.Reserve(32);

	contactListener = new ContactListener();
	destructorListener = new DestructorListener();

	b2Vec2 gravity(0.0f, -9.8f);
	world = new b2World(gravity);
	world->SetContinuousPhysics(true);
	world->SetContactListener(contactListener);
	world->SetDestructionListener(destructorListener);

	b2BodyDef bodyDef;
	bodyDef.type = b2BodyType::b2_dynamicBody;
	worldBody = world->CreateBody(&bodyDef);
}

Box2DPhysics::~Box2DPhysics()
{
	world->DestroyBody(worldBody);
	worldBody = nullptr;

	delete world;
	world = nullptr;

	delete destructorListener;
	destructorListener = nullptr;

	delete contactListener;
	contactListener = nullptr;
}

void Box2DPhysics::Update(float deltaTime)
{
	fixedUpdateTimeAccumulator += deltaTime;
	while(fixedUpdateTimeAccumulator >= fixedStepTime)
	{
		fixedUpdateTimeAccumulator -= fixedStepTime;

		contactsBegin.Clear();
		contactsEnd.Clear();

		world->Step(fixedStepTime, velocityIterations, positionIterations);

		HandleCollisionEnds();
		HandleCollisionBegins();
	}

	UpdateComponents();
}

PhysicsComponent& Box2DPhysics::CreatePhysicsComponentOnObject(Object& object)
{
	Box2DPhysicsComponent& component = object.AddComponent<Box2DPhysicsComponent>();
	return component;
}

void Box2DPhysics::SetGravity(const Vector3& gravity)
{
	world->SetGravity(b2Vec2(gravity.GetX(), gravity.GetY()));
}

Vector3 Box2DPhysics::GetGravity() const
{
	const b2Vec2& gravity = world->GetGravity();
	return Vector3(gravity.x, gravity.y, 0.0f);
}

Collision Box2DPhysics::RaycastClosest(const Vector3& p1, const Vector3& p2, int layerMask)
{
	const std::pair<b2Vec2, b2Vec2> ray = GetValidRay(p1, p2);

	RaycastClosestCallback callback(layerMask);
	world->RayCast(&callback, ray.first, ray.second);

	return callback.collision;
}

bool Box2DPhysics::RaycastAny(const Vector3& p1, const Vector3& p2, int layerMask)
{
	const std::pair<b2Vec2, b2Vec2> ray = GetValidRay(p1, p2);

	RaycastAnyCallback callback(layerMask);
	world->RayCast(&callback, ray.first, ray.second);

	return callback.hasHit;
}

Array<Collision> Box2DPhysics::RaycastAll(const Vector3& p1, const Vector3& p2, int layerMask)
{
	const std::pair<b2Vec2, b2Vec2> ray = GetValidRay(p1, p2);

	RaycastAllCallback callback(layerMask);
	world->RayCast(&callback, ray.first, ray.second);

	return callback.collisions;
}

void Box2DPhysics::SetVelocityIterations(int iterations)
{
	velocityIterations = iterations;
}

int Box2DPhysics::GetVelocityIterations() const
{
	return velocityIterations;
}

void Box2DPhysics::SetPositionIterations(int iterations)
{
	positionIterations = iterations;
}

int Box2DPhysics::GetPositionIterations() const
{
	return positionIterations;
}

b2World* Box2DPhysics::GetWorld() const
{
	return world;
}

b2Body* Box2DPhysics::GetWorldBody() const
{
	return worldBody;
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
				ScriptComponent *scriptComponent = collision.component->GetObject().GetComponent<ScriptComponent>();
				if(scriptComponent != nullptr)
				{
					scriptComponent->Call("OnTriggerEnter", collision.incomingComponent);
				}
			}

			if(collision.incomingComponent->IsTrigger())
			{
				ScriptComponent *scriptComponent = collision.incomingComponent->GetObject().GetComponent<ScriptComponent>();
				if(scriptComponent != nullptr)
				{
					scriptComponent->Call("OnTriggerEnter", collision.component);
				}
			}
		}
		else
		{
			// Collision event (sent to both components)
			ScriptComponent *scriptComponent = collision.component->GetObject().GetComponent<ScriptComponent>();
			if(scriptComponent != nullptr)
			{
				scriptComponent->Call("OnCollisionEnter", &collision);
			}

			scriptComponent = collision.incomingComponent->GetObject().GetComponent<ScriptComponent>();
			if(scriptComponent != nullptr)
			{
				std::swap(collision.component, collision.incomingComponent);
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
				ScriptComponent *scriptComponent = collision.component->GetObject().GetComponent<ScriptComponent>();
				if(scriptComponent != nullptr)
				{
					scriptComponent->Call("OnTriggerExit", collision.incomingComponent);
				}
			}

			if(collision.incomingComponent->IsTrigger())
			{
				ScriptComponent *scriptComponent = collision.incomingComponent->GetObject().GetComponent<ScriptComponent>();
				if(scriptComponent != nullptr)
				{
					scriptComponent->Call("OnTriggerExit", collision.component);
				}
			}
		}
		else
		{
			// Collision event (sent to both components)
			ScriptComponent *scriptComponent = collision.component->GetObject().GetComponent<ScriptComponent>();
			if(scriptComponent != nullptr)
			{
				scriptComponent->Call("OnCollisionExit", &collision);
			}

			scriptComponent = collision.incomingComponent->GetObject().GetComponent<ScriptComponent>();
			if(scriptComponent != nullptr)
			{
				std::swap(collision.component, collision.incomingComponent);
				scriptComponent->Call("OnCollisionExit", &collision);
			}
		}
	}
}
