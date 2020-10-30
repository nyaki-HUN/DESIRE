#include "stdafx_Bullet.h"
#include "BulletCallbacks.h"

#include "BulletPhysics.h"
#include "BulletPhysicsComponent.h"
#include "BulletVectormathExt.h"

#include "Engine/Core/Memory/MemorySystem.h"
#include "Engine/Core/Object.h"

#include "Engine/Script/ScriptComponent.h"

void* BulletCallbacks::MallocWrapper(size_t size)
{
	return MemorySystem::Alloc(size);
}

void* BulletCallbacks::AlignedMallocWrapper(size_t size, int alignment)
{
	return MemorySystem::Alloc(size, alignment);
}

void BulletCallbacks::FreeWrapper(void* pMemory)
{
	MemorySystem::Free(pMemory);
}

void BulletCallbacks::SimulationTickCallback(btDynamicsWorld* pWorld, float timeStep)
{
	DESIRE_UNUSED(timeStep);

	BulletPhysics& physics = *static_cast<BulletPhysics*>(pWorld->getWorldUserInfo());

	const int numManifolds = physics.m_spDispatcher->getNumManifolds();
	for(int manifoldIdx = 0; manifoldIdx < numManifolds; ++manifoldIdx)
	{
		Collision collision;

		const btPersistentManifold* pManifold = physics.m_spDispatcher->getManifoldByIndexInternal(manifoldIdx);
		const btRigidBody* pBody0 = static_cast<const btRigidBody*>(pManifold->getBody0());
		const btRigidBody* pBody1 = static_cast<const btRigidBody*>(pManifold->getBody1());
		collision.pointCount = std::min(pManifold->getNumContacts(), Collision::kMaxContactPoints);
		for(int i = 0; i < collision.pointCount; ++i)
		{
			const btManifoldPoint& pt = pManifold->getContactPoint(i);
			if(pt.getDistance() < 0.0f)
			{
				collision.contactPoints[i] = GetVector3(pt.getPositionWorldOnB());
				collision.contactNormals[i] = GetVector3(pt.m_normalWorldOnB);
			}
		}

		collision.pComponent = static_cast<BulletPhysicsComponent*>(pBody0->getUserPointer());;
		collision.pIncomingComponent = static_cast<BulletPhysicsComponent*>(pBody1->getUserPointer());;

		ScriptComponent* pScriptComponent = collision.pComponent->GetObject().GetComponent<ScriptComponent>();
		if(pScriptComponent != nullptr)
		{
			pScriptComponent->Call("OnCollisionEnter", &collision);
		}

		pScriptComponent = collision.pIncomingComponent->GetObject().GetComponent<ScriptComponent>();
		if(pScriptComponent != nullptr)
		{
			std::swap(collision.pComponent, collision.pIncomingComponent);
			pScriptComponent->Call("OnCollisionEnter", &collision);
		}
	}
}
