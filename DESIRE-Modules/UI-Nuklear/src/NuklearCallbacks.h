#pragma once

class NuklearCallbacks
{
public:
	static void* MallocWrapper(nk_handle userHandle, void* pMemory, nk_size size);
	static void FreeWrapper(nk_handle userHandle, void* pMemory);

	static void ClipboardCopy(nk_handle userHandle, const char* pText, int len);
	static void ClipboardPaste(nk_handle userHandle, nk_text_edit* pEdit);
};
