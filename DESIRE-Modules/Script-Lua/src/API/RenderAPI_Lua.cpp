#include "stdafx_Lua.h"
#include "API/LuaScriptAPI.h"

#include "Engine/Core/Object.h"

#include "Engine/Render/Render.h"
#include "Engine/Render/RenderComponent.h"

void RegisterRenderAPI_Lua(sol::state_view& lua)
{
	if(Modules::Render == nullptr)
	{
		return;
	}

	// RenderComponent
	auto renderComponent = lua.new_usertype<RenderComponent>("RenderComponent", sol::base_classes, sol::bases<Component>());
	renderComponent.set("SetLayer", &RenderComponent::SetLayer);
	renderComponent.set("GetLayer", &RenderComponent::GetLayer);
	renderComponent.set("SetVisible", &RenderComponent::SetVisible);
	renderComponent.set("IsVisible", &RenderComponent::IsVisible);

	lua["Object"]["GetRenderComponent"] = &Object::GetComponent<RenderComponent>;

	// Render
	auto irender = lua.new_usertype<Render>("IRender");
	lua["Render"] = Modules::Render.get();
}
