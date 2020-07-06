#include "stdafx_Nuklear.h"
#include "NuklearCallbacks.h"

#include "Engine/Application/OSWindow.h"

#include "Engine/Core/Memory/MemorySystem.h"
#include "Engine/Core/String/DynamicString.h"

void* NuklearCallbacks::MallocWrapper(nk_handle userHandle, void* pMemory, nk_size size)
{
	DESIRE_UNUSED(userHandle);
	return MemorySystem::Alloc(size);
}

void NuklearCallbacks::FreeWrapper(nk_handle userHandle, void* pMemory)
{
	DESIRE_UNUSED(userHandle);
	MemorySystem::Free(pMemory);
}

void NuklearCallbacks::ClipboardCopy(nk_handle userHandle, const char* pText, int len)
{
    OSWindow* pWindow = static_cast<OSWindow*>(userHandle.ptr);
    if(pWindow != nullptr)
    {
        pWindow->SetClipboardString(String(pText, len));
    }
}

void NuklearCallbacks::ClipboardPaste(nk_handle userHandle, nk_text_edit* pEdit)
{
    OSWindow* pWindow = static_cast<OSWindow*>(userHandle.ptr);
    if(pWindow != nullptr)
    {
        DynamicString string;
        pWindow->GetClipboardString(string);

        nk_textedit_paste(pEdit, string.Str(), static_cast<int>(string.LengthUTF8()));
    }
}
