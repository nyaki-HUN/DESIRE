#include "stdafx_Lua.h"
#include "API/LuaScriptAPI.h"

#include "Engine/Core/Object.h"

#include "Engine/Render/Render.h"
#include "Engine/Render/RenderComponent.h"

void RegisterRenderAPI_Lua(sol::state_view& lua)
{
	Render* render = Modules::Render.get();
	if(render == nullptr)
	{
		return;
	}

	// RenderComponent
	auto renderComponent = lua.new_usertype<RenderComponent>("RenderComponent", sol::base_classes, sol::bases<Component>());
	renderComponent.set("layer", sol::property(&RenderComponent::GetLayer, &RenderComponent::SetLayer));

	lua["Object"]["GetRenderComponent"] = &Object::GetComponent<RenderComponent>;

	// Render
	auto irender = lua.new_usertype<Render>("IRender");
	lua["Render"] = render;
}
