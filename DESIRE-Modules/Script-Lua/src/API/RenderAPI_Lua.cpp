#include "API/LuaScriptAPI.h"

#include "Engine/Modules.h"
#include "Engine/Core/Object.h"
#include "Engine/Render/Render.h"
#include "Engine/Render/RenderComponent.h"

void RegisterRenderAPI_Lua(lua_State* L)
{
	Render* render = Modules::Render.get();
	if(render == nullptr)
	{
		return;
	}

	// RenderComponent
	luabridge::getGlobalNamespace(L).deriveClass<RenderComponent, Component>("RenderComponent")
		.addProperty("layer", &RenderComponent::GetLayer, &RenderComponent::SetLayer)
		.endClass();

	luabridge::getGlobalNamespace(L).beginClass<Object>("Object")
		.addFunction<RenderComponent* (Object::*)() const>("GetRenderComponent", &Object::GetComponent<RenderComponent>)
		.endClass();

	// Render
	luabridge::getGlobalNamespace(L).beginClass<Render>("IRender")
		.endClass();

	luabridge::setGlobal(L, render, "Render");
}
