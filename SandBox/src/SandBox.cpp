#include "SandBox.h"
#include "SimpleRotateScript.h"
#include "Core/Object.h"
#include "Core/Log.h"
#include "Core/fs/FileSystem.h"
#include "Core/fs/IReadFile.h"
#include "Input/Input.h"
#include "Render/Render.h"
#include "UI/ImGuiImpl.h"
#include "Resource/ResourceManager.h"
#include "Script/IScriptSystem.h"
#include "Component/SceneNodeComponent.h"
#include "Component/ScriptComponent.h"

enum EAction
{
	EXIT,
	FIRE,
	MOUSE_X,
	MOUSE_Y
}; 

SandBox::SandBox()
	: window(nullptr)
	, scriptedObject(nullptr)
	, cubeObj(nullptr)
{

}

SandBox::~SandBox()
{

}

void SandBox::Init(IWindow *mainWindow)
{
	window = mainWindow;

	FileSystem::Get()->AddZipFileSource("zip.zip");
//	FileSystem::Get()->AddZipFileSource("zip.zip", FileSystem::FLAG_IGNORE_CASE | FileSystem::FLAG_IGNORE_PATH);

	ReadFilePtr file = FileSystem::Get()->Open("asd.txt");
	ReadFilePtr file2 = FileSystem::Get()->Open("qwe.txt");
	ReadFilePtr file3 = FileSystem::Get()->Open("dir/qwe.txt");

	//////////

	scriptedObject = new Object("Obj_1");
	scriptedObject->AddComponent(IScriptSystem::Get()->CreateScriptComponent("TestScript"));
	scriptedObject->AddComponent(new SceneNodeComponent());

	ScriptComponent *scriptComp = scriptedObject->GetComponent<ScriptComponent>();
	if(scriptComp != nullptr)
	{
		scriptComp->CallByType(ScriptComponent::EBuiltinFuncType::INIT);

		scriptComp->Call("Function1");
		scriptComp->Call("Function2", 12.3f, 100);
		String stringValue = "TEST";
		scriptComp->Call("Function3", stringValue);
	}

	//////////

	REGISTER_NATIVE_SCRIPT(SimpleRotateScript);

	cubeObj = new Object("Obj_2");
	cubeObj->AddComponent(IScriptSystem::Get()->CreateScriptComponent("SimpleRotateScript"));
	cubeObj->AddComponent(new SceneNodeComponent());

	//////////

	Mesh *mesh = ResourceManager::Get()->LoadMesh("data/meshes/sibenik/sibenik.obj");

	Texture *texture = ResourceManager::Get()->LoadTexture("data/meshes/sibenik/mramor6x6.png");
	if(texture != nullptr)
	{
		Render::Get()->Bind(texture);
	}

	//////////

	for(const Keyboard& keyboard : Input::Get()->GetKeyboards())
	{
		inputMapping.MapButton(EAction::EXIT, keyboard, KEY_ESCAPE);
		inputMapping.MapButton(EAction::FIRE, keyboard, KEY_SPACE);
	}

	for(const GameController& gamepad : Input::Get()->GetControllers())
	{
		inputMapping.MapButton(EAction::FIRE, gamepad, GameController::BTN_X);
	}

	//////////

	LOG_DEBUG("Init done");
}

void SandBox::Kill()
{
	delete scriptedObject;
}

void SandBox::Update()
{
	if(inputMapping.GetPressedCount(EAction::EXIT))
	{
		LOG_MESSAGE("!EXIT!");
		return;
	}

	if(inputMapping.IsDown(EAction::FIRE))
	{
		LOG_MESSAGE("Fire is down");
	}

	Render::Get()->BeginFrame(window);
	ImGuiImpl::Get()->NewFrame(window);

	ImGui::SetNextWindowSize(ImVec2(200, 100), ImGuiSetCond_FirstUseEver);
	ImGui::Begin("Another Window");
	ImGui::Text("Hello");
	ImGui::End();

	ImGuiImpl::Get()->EndFrame();
	Render::Get()->EndFrame();
}
