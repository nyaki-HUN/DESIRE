#pragma once

#include "BulletDynamics/Vehicle/btVehicleRaycaster.h"

class btDynamicsWorld;

class BulletVehicleRaycaster : public btVehicleRaycaster
{
public:
	BulletVehicleRaycaster(btDynamicsWorld* pWorld);

	void* castRay(const btVector3& from, const btVector3& to, btVehicleRaycasterResult& result) override;

private:
	btDynamicsWorld* m_pDynamicsWorld;
};
