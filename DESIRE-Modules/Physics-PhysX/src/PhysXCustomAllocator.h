#pragma once

class PhysXCustomAllocator : public physx::PxAllocatorCallback
{
public:
	void* allocate(size_t size, const char* pTypeName, const char* pFilename, int line) override;
	void deallocate(void* pMemory) override;
};
