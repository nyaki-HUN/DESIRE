#include "API/SquirrelScriptAPI.h"

#include "Engine/Core/Object.h"

#include "Engine/Render/Render.h"
#include "Engine/Render/RenderComponent.h"

void RegisterRenderAPI_Squirrel(Sqrat::RootTable& rootTable)
{
	Render* render = Modules::Render.get();
	if(render == nullptr)
	{
		return;
	}

	HSQUIRRELVM vm = rootTable.GetVM();

	// RenderComponent
	rootTable.Bind("RenderComponent", Sqrat::DerivedClass<RenderComponent, Component, Sqrat::NoConstructor<RenderComponent>>(vm, "RenderComponent")
		.Prop("layer", &RenderComponent::GetLayer, &RenderComponent::SetLayer)
	);

	Sqrat::Class<Object, Sqrat::NoConstructor<Object>>(vm, "Object", false).Func("GetRenderComponent", &Object::GetComponent<RenderComponent>);

	// Render
	rootTable.Bind("IRender", Sqrat::Class<Render, Sqrat::NoConstructor<Render>>(vm, "IRender")
	);
	rootTable.SetInstance("Render", render);
}
