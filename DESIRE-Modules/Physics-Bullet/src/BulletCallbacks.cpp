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

void BulletCallbacks::FreeWrapper(void* ptr)
{
	MemorySystem::Free(ptr);
}

void BulletCallbacks::SimulationTickCallback(btDynamicsWorld* world, float timeStep)
{
	DESIRE_UNUSED(timeStep);

	BulletPhysics* physics = static_cast<BulletPhysics*>(world->getWorldUserInfo());

	const int numManifolds = physics->dispatcher->getNumManifolds();
	for(int manifoldIdx = 0; manifoldIdx < numManifolds; ++manifoldIdx)
	{
		Collision collision;

		const btPersistentManifold* manifold = physics->dispatcher->getManifoldByIndexInternal(manifoldIdx);
		const btRigidBody* body0 = static_cast<const btRigidBody*>(manifold->getBody0());
		const btRigidBody* body1 = static_cast<const btRigidBody*>(manifold->getBody1());
		collision.pointCount = std::min(manifold->getNumContacts(), Collision::kMaxContactPoints);
		for(int i = 0; i < collision.pointCount; ++i)
		{
			const btManifoldPoint& pt = manifold->getContactPoint(i);
			if(pt.getDistance() < 0.0f)
			{
				collision.contactPoints[i] = GetVector3(pt.getPositionWorldOnB());
				collision.contactNormals[i] = GetVector3(pt.m_normalWorldOnB);
			}
		}

		collision.component = static_cast<BulletPhysicsComponent*>(body0->getUserPointer());;
		collision.incomingComponent = static_cast<BulletPhysicsComponent*>(body1->getUserPointer());;

		ScriptComponent* scriptComponent = collision.component->GetObject().GetComponent<ScriptComponent>();
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
