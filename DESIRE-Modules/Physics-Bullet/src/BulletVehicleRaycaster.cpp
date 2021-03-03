#include "stdafx_Bullet.h"
#include "BulletVehicleRaycaster.h"

#include "Engine/Physics/Physics.h"

BulletVehicleRaycaster::BulletVehicleRaycaster(btDynamicsWorld* pWorld)
	: m_pDynamicsWorld(pWorld)
{
}

void* BulletVehicleRaycaster::castRay(const btVector3& from, const btVector3& to, btVehicleRaycasterResult& result)
{
	btCollisionWorld::ClosestRayResultCallback rayCallback(from, to);
	rayCallback.m_collisionFilterGroup = Physics::MASK_WHEEL;
	rayCallback.m_collisionFilterMask = Physics::MASK_DEFAULT;

	m_pDynamicsWorld->rayTest(from, to, rayCallback);

	if(rayCallback.hasHit())
	{
		const btRigidBody* pBody = btRigidBody::upcast(rayCallback.m_collisionObject);
		if(pBody && pBody->hasContactResponse())
		{
			result.m_hitPointInWorld = rayCallback.m_hitPointWorld;
			result.m_hitNormalInWorld = rayCallback.m_hitNormalWorld;
			result.m_hitNormalInWorld.normalize();
			result.m_distFraction = rayCallback.m_closestHitFraction;
			return const_cast<btRigidBody*>(pBody);
		}
	}

	return nullptr;
}
