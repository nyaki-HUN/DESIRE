#include "stdafx_Nuklear.h"
#include "NuklearUI.h"

#include "NuklearCallbacks.h"

#include "Engine/Application/Application.h"
#include "Engine/Application/OSWindow.h"

#include "Engine/Core/Math/Vector3.h"
#include "Engine/Core/String/String.h"
#include "Engine/Core/Timer.h"

#include "Engine/Input/Input.h"

NuklearUI::NuklearUI()
	: allocator(std::make_unique<nk_allocator>())
{
	allocator->alloc = &NuklearCallbacks::MallocWrapper;
	allocator->free = &NuklearCallbacks::FreeWrapper;
	allocator->userdata = nk_handle_ptr(this);
}

NuklearUI::~NuklearUI()
{
}

void NuklearUI::Init()
{
	ctx = std::make_unique<nk_context>();
	nk_init(ctx.get(), allocator.get(), nullptr);
}

void NuklearUI::Kill()
{
	nk_free(ctx.get());
	ctx = nullptr;
}

void NuklearUI::BeginFrame(OSWindow* pWindow)
{
	ctx->delta_time_seconds = Modules::Application->GetTimer()->GetSecDelta();

	nk_input_begin(ctx.get());

	// Keyboard
	for(int i = 0; i < NK_KEY_MAX; i++)
	{
		ctx->input.keyboard.keys[i].down = 0;
	}

	for(const Keyboard& keyboard : Modules::Input->GetKeyboards())
	{
		const bool keyCtrl = keyboard.IsDown(KEY_LCONTROL) || keyboard.IsDown(KEY_RCONTROL);

		// Key down
		ctx->input.keyboard.keys[NK_KEY_SHIFT].down += keyboard.IsDown(KEY_LSHIFT) || keyboard.IsDown(KEY_RSHIFT);
		ctx->input.keyboard.keys[NK_KEY_SHIFT].clicked += keyboard.GetPressedCount(KEY_LSHIFT) + keyboard.GetPressedCount(KEY_RSHIFT);

		ctx->input.keyboard.keys[NK_KEY_CTRL].down += keyCtrl;
		ctx->input.keyboard.keys[NK_KEY_CTRL].clicked += keyboard.GetPressedCount(KEY_LCONTROL) + keyboard.GetPressedCount(KEY_RCONTROL);

		ctx->input.keyboard.keys[NK_KEY_DEL].down += keyboard.IsDown(KEY_DELETE);
		ctx->input.keyboard.keys[NK_KEY_DEL].clicked += keyboard.GetPressedCount(KEY_DELETE);

		ctx->input.keyboard.keys[NK_KEY_ENTER].down += keyboard.IsDown(KEY_RETURN) || keyboard.IsDown(KEY_NUMPADENTER);
		ctx->input.keyboard.keys[NK_KEY_ENTER].clicked += keyboard.GetPressedCount(KEY_RETURN) + keyboard.GetPressedCount(KEY_NUMPADENTER);

		ctx->input.keyboard.keys[NK_KEY_TAB].down += keyboard.IsDown(KEY_TAB);
		ctx->input.keyboard.keys[NK_KEY_TAB].clicked += keyboard.GetPressedCount(KEY_TAB);

		ctx->input.keyboard.keys[NK_KEY_BACKSPACE].down += keyboard.IsDown(KEY_BACKSPACE);
		ctx->input.keyboard.keys[NK_KEY_BACKSPACE].clicked += keyboard.GetPressedCount(KEY_BACKSPACE);

//		ctx->input.keyboard.keys[NK_KEY_TEXT_INSERT_MODE].down +=
//		ctx->input.keyboard.keys[NK_KEY_TEXT_INSERT_MODE].clicked +=

//		ctx->input.keyboard.keys[NK_KEY_TEXT_REPLACE_MODE].down +=
//		ctx->input.keyboard.keys[NK_KEY_TEXT_REPLACE_MODE].clicked +=

//		ctx->input.keyboard.keys[NK_KEY_TEXT_RESET_MODE].down +=
//		ctx->input.keyboard.keys[NK_KEY_TEXT_RESET_MODE].clicked +=

		ctx->input.keyboard.keys[NK_KEY_TEXT_LINE_START].down += keyboard.IsDown(KEY_HOME);
		ctx->input.keyboard.keys[NK_KEY_TEXT_LINE_START].clicked += keyboard.GetPressedCount(KEY_HOME);

		ctx->input.keyboard.keys[NK_KEY_TEXT_LINE_END].down += keyboard.IsDown(KEY_END);
		ctx->input.keyboard.keys[NK_KEY_TEXT_LINE_END].clicked += keyboard.GetPressedCount(KEY_END);

//		ctx->input.keyboard.keys[NK_KEY_TEXT_START].down +=
//		ctx->input.keyboard.keys[NK_KEY_TEXT_START].clicked +=

//		ctx->input.keyboard.keys[NK_KEY_TEXT_END].down +=
//		ctx->input.keyboard.keys[NK_KEY_TEXT_END].clicked +=

		ctx->input.keyboard.keys[NK_KEY_SCROLL_START].down += keyboard.IsDown(KEY_HOME);
		ctx->input.keyboard.keys[NK_KEY_SCROLL_START].clicked += keyboard.GetPressedCount(KEY_HOME);

		ctx->input.keyboard.keys[NK_KEY_SCROLL_END].down += keyboard.IsDown(KEY_END);
		ctx->input.keyboard.keys[NK_KEY_SCROLL_END].clicked += keyboard.GetPressedCount(KEY_END);

		ctx->input.keyboard.keys[NK_KEY_SCROLL_DOWN].down += keyboard.GetPressedCount(KEY_PGDOWN);
		ctx->input.keyboard.keys[NK_KEY_SCROLL_DOWN].clicked += keyboard.GetPressedCount(KEY_PGDOWN);

		ctx->input.keyboard.keys[NK_KEY_SCROLL_UP].down += keyboard.GetPressedCount(KEY_PGUP);
		ctx->input.keyboard.keys[NK_KEY_SCROLL_UP].clicked += keyboard.GetPressedCount(KEY_PGUP);

		if(keyCtrl)
		{
			ctx->input.keyboard.keys[NK_KEY_COPY].down += keyboard.IsDown(KEY_C);
			ctx->input.keyboard.keys[NK_KEY_COPY].clicked += keyboard.GetPressedCount(KEY_C);

			ctx->input.keyboard.keys[NK_KEY_CUT].down += keyboard.IsDown(KEY_X);
			ctx->input.keyboard.keys[NK_KEY_CUT].clicked += keyboard.GetPressedCount(KEY_X);

			ctx->input.keyboard.keys[NK_KEY_PASTE].down += keyboard.IsDown(KEY_V);
			ctx->input.keyboard.keys[NK_KEY_PASTE].clicked += keyboard.GetPressedCount(KEY_V);

			ctx->input.keyboard.keys[NK_KEY_TEXT_UNDO].down += keyboard.IsDown(KEY_Z);
			ctx->input.keyboard.keys[NK_KEY_TEXT_UNDO].clicked += keyboard.GetPressedCount(KEY_Z);

			ctx->input.keyboard.keys[NK_KEY_TEXT_REDO].down += keyboard.IsDown(KEY_Y);
			ctx->input.keyboard.keys[NK_KEY_TEXT_REDO].clicked += keyboard.GetPressedCount(KEY_Y);

			ctx->input.keyboard.keys[NK_KEY_TEXT_SELECT_ALL].down += keyboard.IsDown(KEY_A);
			ctx->input.keyboard.keys[NK_KEY_TEXT_SELECT_ALL].clicked += keyboard.GetPressedCount(KEY_A);

			ctx->input.keyboard.keys[NK_KEY_TEXT_WORD_LEFT].down += keyboard.IsDown(KEY_LEFT);
			ctx->input.keyboard.keys[NK_KEY_TEXT_WORD_LEFT].clicked += keyboard.GetPressedCount(KEY_LEFT);

			ctx->input.keyboard.keys[NK_KEY_TEXT_WORD_RIGHT].down += keyboard.IsDown(KEY_RIGHT);
			ctx->input.keyboard.keys[NK_KEY_TEXT_WORD_RIGHT].clicked += keyboard.GetPressedCount(KEY_RIGHT);

		}
		else
		{
			ctx->input.keyboard.keys[NK_KEY_UP].down += keyboard.IsDown(KEY_UP);
			ctx->input.keyboard.keys[NK_KEY_UP].clicked += keyboard.GetPressedCount(KEY_UP);

			ctx->input.keyboard.keys[NK_KEY_DOWN].down += keyboard.IsDown(KEY_DOWN);
			ctx->input.keyboard.keys[NK_KEY_DOWN].clicked += keyboard.GetPressedCount(KEY_DOWN);

			ctx->input.keyboard.keys[NK_KEY_LEFT].down += keyboard.IsDown(KEY_LEFT);
			ctx->input.keyboard.keys[NK_KEY_LEFT].clicked += keyboard.GetPressedCount(KEY_LEFT);

			ctx->input.keyboard.keys[NK_KEY_RIGHT].down += keyboard.IsDown(KEY_RIGHT);
			ctx->input.keyboard.keys[NK_KEY_RIGHT].clicked += keyboard.GetPressedCount(KEY_RIGHT);
		}
	}

	const char* typedCharacters = Modules::Input->GetTypingCharacters();
	for(int i = 0; typedCharacters[i] != '\0'; ++i)
	{
		nk_input_unicode(ctx.get(), typedCharacters[i]);
	}

	// Mouse
	const Vector2& mousePos = Modules::Input->GetOsMouseCursorPos();
	const int x = static_cast<int>(mousePos.GetX());
	const int y = static_cast<int>(mousePos.GetY());
	for(const Mouse& mouse : Modules::Input->GetMouses())
	{
		nk_input_button(ctx.get(), NK_BUTTON_LEFT, x, y, mouse.IsDown(Mouse::Button_Left));
		nk_input_button(ctx.get(), NK_BUTTON_MIDDLE, x, y, mouse.IsDown(Mouse::Button_Middle));
		nk_input_button(ctx.get(), NK_BUTTON_RIGHT, x, y, mouse.IsDown(Mouse::Button_Right));
	}

	nk_input_end(ctx.get());
}

void NuklearUI::EndFrame()
{
}

void NuklearUI::Render()
{
}

void NuklearUI::BeginWindow(const String& label)
{
}

void NuklearUI::EndWindow()
{
}

void NuklearUI::Text(const String& label)
{
}

bool NuklearUI::Button(const String& label, const Vector2& size)
{
	return nk_button_label(ctx.get(), label.Str());
}

bool NuklearUI::RadioButton(const String& label, bool isActive)
{
	return false;
}

bool NuklearUI::InputField(const String& label, float& value)
{
	return false;
}

bool NuklearUI::InputField(const String& label, Vector3& value)
{
	return false;
}

bool NuklearUI::Slider(const String& label, int32_t& value, int32_t minValue, int32_t maxValue)
{
	return false;
}

bool NuklearUI::Slider(const String& label, float& value, float minValue, float maxValue)
{
	return false;
}

bool NuklearUI::Checkbox(const String& label, bool& isChecked)
{
	return false;
}

void NuklearUI::SameLine()
{
}
