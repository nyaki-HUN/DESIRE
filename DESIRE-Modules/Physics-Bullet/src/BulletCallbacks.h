#pragma once

class BulletCallbacks
{
public:
	static void* MallocWrapper(size_t size);
	static void* AlignedMallocWrapper(size_t size, int alignment);
	static void FreeWrapper(void* ptr);

	static void SimulationTickCallback(btDynamicsWorld* world, float timeStep);
};
