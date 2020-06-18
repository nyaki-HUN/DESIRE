#include "stdafx_imgui.h"
#include "ImGuiCallbacks.h"

#include "Engine/Core/Memory/MemorySystem.h"

void* ImGuiCallbacks::MallocWrapper(size_t size, void* pUserData)
{
	DESIRE_UNUSED(pUserData);
	return MemorySystem::Alloc(size);
}

void ImGuiCallbacks::FreeWrapper(void* pMemory, void* pUserData)
{
	DESIRE_UNUSED(pUserData);
	MemorySystem::Free(pMemory);
}
