#include "stdafx_Box2D.h"
#include "Box2DPhysics.h"

#include "Box2DPhysicsComponent.h"
#include "ContactListener.h"
#include "DestructorListener.h"
#include "RaycastCallbacks.h"

#include "Engine/Core/Memory/MemorySystem.h"
#include "Engine/Core/Object.h"

#include "Engine/Physics/PhysicsComponent.h"

#include "Engine/Script/ScriptComponent.h"

static std::pair<b2Vec2, b2Vec2> GetValidRay(const Vector3& p1, const Vector3& p2)
{
	std::pair<b2Vec2, b2Vec2> ray(b2Vec2(p1.GetX(), p1.GetY()), b2Vec2(p2.GetX(), p2.GetY()));

	if(ray.first.x == ray.second.x && ray.first.y == ray.second.y)
	{
		constexpr float kEpsilon = 0.001f;
		ray.first.x += kEpsilon;
		ray.first.y += kEpsilon;
	}

	return ray;
}

// Memory functions for Box2D
void* b2Alloc(int32 size)	{ return MemorySystem::Alloc(size); }
void b2Free(void* mem)		{ MemorySystem::Free(mem); }

Box2DPhysics::Box2DPhysics()
{
	contactsBegin.Reserve(32);
	contactsEnd.Reserve(32);

	m_pContactListener = new ContactListener();
	m_pDestructorListener = new DestructorListener();

	b2Vec2 gravity(0.0f, -9.8f);
	m_pWorld = new b2World(gravity);
	m_pWorld->SetContinuousPhysics(true);
	m_pWorld->SetContactListener(m_pContactListener);
	m_pWorld->SetDestructionListener(m_pDestructorListener);

	b2BodyDef bodyDef;
	bodyDef.type = b2BodyType::b2_dynamicBody;
	m_pWorldBody = m_pWorld->CreateBody(&bodyDef);
}

Box2DPhysics::~Box2DPhysics()
{
	m_pWorld->DestroyBody(m_pWorldBody);
	m_pWorldBody = nullptr;

	delete m_pWorld;
	m_pWorld = nullptr;

	delete m_pDestructorListener;
	m_pDestructorListener = nullptr;

	delete m_pContactListener;
	m_pContactListener = nullptr;
}

void Box2DPhysics::Update(float deltaTime)
{
	m_fixedUpdateTimeAccumulator += deltaTime;
	while(m_fixedUpdateTimeAccumulator >= m_fixedStepTime)
	{
		m_fixedUpdateTimeAccumulator -= m_fixedStepTime;

		contactsBegin.Clear();
		contactsEnd.Clear();

		m_pWorld->Step(m_fixedStepTime, m_velocityIterations, m_positionIterations);

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
	m_pWorld->SetGravity(b2Vec2(gravity.GetX(), gravity.GetY()));
}

Vector3 Box2DPhysics::GetGravity() const
{
	const b2Vec2& gravity = m_pWorld->GetGravity();
	return Vector3(gravity.x, gravity.y, 0.0f);
}

Collision Box2DPhysics::RaycastClosest(const Vector3& p1, const Vector3& p2, int layerMask)
{
	const std::pair<b2Vec2, b2Vec2> ray = GetValidRay(p1, p2);

	RaycastClosestCallback callback(layerMask);
	m_pWorld->RayCast(&callback, ray.first, ray.second);

	return callback.collision;
}

bool Box2DPhysics::RaycastAny(const Vector3& p1, const Vector3& p2, int layerMask)
{
	const std::pair<b2Vec2, b2Vec2> ray = GetValidRay(p1, p2);

	RaycastAnyCallback callback(layerMask);
	m_pWorld->RayCast(&callback, ray.first, ray.second);

	return callback.hasHit;
}

Array<Collision> Box2DPhysics::RaycastAll(const Vector3& p1, const Vector3& p2, int layerMask)
{
	const std::pair<b2Vec2, b2Vec2> ray = GetValidRay(p1, p2);

	RaycastAllCallback callback(layerMask);
	m_pWorld->RayCast(&callback, ray.first, ray.second);

	return callback.collisions;
}

void Box2DPhysics::SetVelocityIterations(int iterations)
{
	m_velocityIterations = iterations;
}

int Box2DPhysics::GetVelocityIterations() const
{
	return m_velocityIterations;
}

void Box2DPhysics::SetPositionIterations(int iterations)
{
	m_positionIterations = iterations;
}

int Box2DPhysics::GetPositionIterations() const
{
	return m_positionIterations;
}

b2World& Box2DPhysics::GetWorld() const
{
	return *m_pWorld;
}

b2Body* Box2DPhysics::GetWorldBody() const
{
	return m_pWorldBody;
}

void Box2DPhysics::HandleCollisionBegins()
{
	for(Collision& collision : contactsBegin)
	{
		if(collision.pComponent->IsTrigger() || collision.pIncomingComponent->IsTrigger())
		{
			// Trigger event (only sent to triggers)
			if(collision.pComponent->IsTrigger())
			{
				ScriptComponent* pScriptComponent = collision.pComponent->GetObject().GetComponent<ScriptComponent>();
				if(pScriptComponent)
				{
					pScriptComponent->Call("OnTriggerEnter", collision.pIncomingComponent);
				}
			}

			if(collision.pIncomingComponent->IsTrigger())
			{
				ScriptComponent* pScriptComponent = collision.pIncomingComponent->GetObject().GetComponent<ScriptComponent>();
				if(pScriptComponent)
				{
					pScriptComponent->Call("OnTriggerEnter", collision.pComponent);
				}
			}
		}
		else
		{
			// Collision event (sent to both components)
			ScriptComponent* pScriptComponent = collision.pComponent->GetObject().GetComponent<ScriptComponent>();
			if(pScriptComponent)
			{
				pScriptComponent->Call("OnCollisionEnter", &collision);
			}

			pScriptComponent = collision.pIncomingComponent->GetObject().GetComponent<ScriptComponent>();
			if(pScriptComponent)
			{
				std::swap(collision.pComponent, collision.pIncomingComponent);
				pScriptComponent->Call("OnCollisionEnter", &collision);
			}
		}
	}
}

void Box2DPhysics::HandleCollisionEnds()
{
	for(Collision& collision : contactsEnd)
	{
		if(collision.pComponent->IsTrigger() || collision.pIncomingComponent->IsTrigger())
		{
			// Trigger event (only sent to triggers)
			if(collision.pComponent->IsTrigger())
			{
				ScriptComponent* pScriptComponent = collision.pComponent->GetObject().GetComponent<ScriptComponent>();
				if(pScriptComponent)
				{
					pScriptComponent->Call("OnTriggerExit", collision.pIncomingComponent);
				}
			}

			if(collision.pIncomingComponent->IsTrigger())
			{
				ScriptComponent* pScriptComponent = collision.pIncomingComponent->GetObject().GetComponent<ScriptComponent>();
				if(pScriptComponent)
				{
					pScriptComponent->Call("OnTriggerExit", collision.pComponent);
				}
			}
		}
		else
		{
			// Collision event (sent to both components)
			ScriptComponent* pScriptComponent = collision.pComponent->GetObject().GetComponent<ScriptComponent>();
			if(pScriptComponent)
			{
				pScriptComponent->Call("OnCollisionExit", &collision);
			}

			pScriptComponent = collision.pIncomingComponent->GetObject().GetComponent<ScriptComponent>();
			if(pScriptComponent)
			{
				std::swap(collision.pComponent, collision.pIncomingComponent);
				pScriptComponent->Call("OnCollisionExit", &collision);
			}
		}
	}
}
