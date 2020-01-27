#include "stdafx_Bullet.h"
#include "BulletVehicleRaycaster.h"

#include "Engine/Physics/Physics.h"

BulletVehicleRaycaster::BulletVehicleRaycaster(btDynamicsWorld* world)
	: dynamicsWorld(world)
{
}

void* BulletVehicleRaycaster::castRay(const btVector3& from, const btVector3& to, btVehicleRaycasterResult& result)
{
	btCollisionWorld::ClosestRayResultCallback rayCallback(from, to);
	rayCallback.m_collisionFilterGroup = Physics::MASK_WHEEL;
	rayCallback.m_collisionFilterMask = Physics::MASK_DEFAULT;

	dynamicsWorld->rayTest(from, to, rayCallback);

	if(rayCallback.hasHit())
	{
		const btRigidBody* body = btRigidBody::upcast(rayCallback.m_collisionObject);
		if(body != nullptr && body->hasContactResponse())
		{
			result.m_hitPointInWorld = rayCallback.m_hitPointWorld;
			result.m_hitNormalInWorld = rayCallback.m_hitNormalWorld;
			result.m_hitNormalInWorld.normalize();
			result.m_distFraction = rayCallback.m_closestHitFraction;
			return (void*)body;
		}
	}

	return nullptr;
}
