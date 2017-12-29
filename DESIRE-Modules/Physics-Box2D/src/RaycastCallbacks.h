#pragma once

#include "Box2D/Dynamics/b2WorldCallbacks.h"
#include "Box2D/Dynamics/b2Fixture.h"

#include <vector>

class PhysicsComponent;

class RaycastCallbackBase : public b2RayCastCallback
{
protected:
	RaycastCallbackBase(int layerMask)
		: layerMask(layerMask)
	{

	}

	bool IsFilteredOut(const b2Fixture *fixture) const
	{
		const b2Filter& filterData = fixture->GetFilterData();
		return (filterData.categoryBits & layerMask) == 0;
	}

	int layerMask;
};

class RaycastClosestCallback : public RaycastCallbackBase
{
public:
	RaycastClosestCallback(int layerMask)
		: RaycastCallbackBase(layerMask)
	{

	}

	float32 ReportFixture(b2Fixture *fixture, const b2Vec2& point, const b2Vec2& normal, float32 fraction) override
	{
		if(IsFilteredOut(fixture))
		{
			return -1;		// Filter out the fixture
		}

		component = static_cast<PhysicsComponent*>(fixture->GetUserData());
		contactPoint = point;
		contactNormal = normal;
		return fraction;	// The ray will be clipped to the current intersection point
	}

	PhysicsComponent *component = nullptr;
	b2Vec2 contactPoint;
	b2Vec2 contactNormal;
};

class RaycastAnyCallback : public RaycastCallbackBase
{
public:
	RaycastAnyCallback(int layerMask)
		: RaycastCallbackBase(layerMask)
	{
	
	}

	float32 ReportFixture(b2Fixture *fixture, const b2Vec2& point, const b2Vec2& normal, float32 fraction) override
	{
		if(IsFilteredOut(fixture))
		{
			return -1;		// Filter out the fixture
		}

		hasHit = true;
		return 0.0f;		// The raycast should be terminated
	}

	bool hasHit = false;
};

class RaycastAllCallback : public RaycastCallbackBase
{
public:
	RaycastAllCallback(int layerMask)
		: RaycastCallbackBase(layerMask)
	{

	}

	float32 ReportFixture(b2Fixture *fixture, const b2Vec2& point, const b2Vec2& normal, float32 fraction) override
	{
		if(IsFilteredOut(fixture))
		{
			return -1;		// Filter out the fixture
		}

		PhysicsComponent *component = static_cast<PhysicsComponent*>(fixture->GetUserData());
		collisions.emplace_back(component, point, normal);
		return 1.0f;		// The raycast should continue as if no hit occurred
	}

	struct CollisionResult
	{
		PhysicsComponent *component = nullptr;
		b2Vec2 contactPoint;
		b2Vec2 contactNormal;

		CollisionResult(PhysicsComponent *component, const b2Vec2& contactPoint, const b2Vec2& contactNormal)
			: component(component)
			, contactPoint(contactPoint)
			, contactNormal(contactNormal)
		{

		}
	};

	std::vector<CollisionResult> collisions;
};
