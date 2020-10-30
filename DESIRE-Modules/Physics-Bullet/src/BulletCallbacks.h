#pragma once

class BulletCallbacks
{
public:
	static void* MallocWrapper(size_t size);
	static void* AlignedMallocWrapper(size_t size, int alignment);
	static void FreeWrapper(void* pMemory);

	static void SimulationTickCallback(btDynamicsWorld* pWorld, float timeStep);
};
