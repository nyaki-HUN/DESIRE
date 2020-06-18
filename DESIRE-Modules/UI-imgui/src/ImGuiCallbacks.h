#pragma once

class ImGuiCallbacks
{
public:
	static void* MallocWrapper(size_t size, void* pUserData);
	static void FreeWrapper(void* pMemory, void* pUserData);
};
