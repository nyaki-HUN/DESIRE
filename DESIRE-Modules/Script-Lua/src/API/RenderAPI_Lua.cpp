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
	lua.new_usertype<RenderComponent>("RenderComponent",
		sol::base_classes, sol::bases<Component>(),
		"layer", sol::property(&RenderComponent::GetLayer, &RenderComponent::SetLayer)
	);

	lua.new_usertype<Object>("Object",
		"GetRenderComponent", &Object::GetComponent<RenderComponent>
	);

	// Render
	lua.new_usertype<Render>("IRender"
	);

	lua.set("Render", render);
}
