#include "stdafx.h"
#include "SandBox.h"

#include "SimpleRotateScript.h"

#include "Engine/Application/ResourceManager.h"

#include "Engine/Compression/FileSourceZip.h"

#include "Engine/Core/Object.h"
#include "Engine/Core/FS/FileSystem.h"
#include "Engine/Core/FS/FileSystemWatcher.h"
#include "Engine/Core/FS/IReadFile.h"
#include "Engine/Core/Math/Vector3.h"
#include "Engine/Core/String/StackString.h"

#include "Engine/Input/Input.h"

#include "Engine/Render/Render.h"

#include "Engine/Script/ScriptSystem.h"
#include "Engine/Script/ScriptComponent.h"

#include "Engine/UI/UI.h"

enum EAction
{
	Exit,
	Fire,
	CameraYaw,
	CameraPitch
};

SandBox::SandBox()
{
}

void SandBox::Init()
{
	LOG_MESSAGE(u8"��");

	ReadFilePtr spZipFile = FileSystem::Get().Open("data/zip.zip");
	if(spZipFile)
	{
		std::unique_ptr<FileSourceZip> spZipFileSource = std::make_unique<FileSourceZip>(std::move(spZipFile), FileSystem::FILESOURCE_IGNORE_PATH | FileSystem::FILESOURCE_IGNORE_CASE);
		if(spZipFileSource->Load())
		{
			FileSystem::Get().AddFileSource(std::move(spZipFileSource));
		}
	}

	ReadFilePtr file = FileSystem::Get().Open("asd.txt");
	ReadFilePtr file2 = FileSystem::Get().Open("qwe.txt");
	ReadFilePtr file3 = FileSystem::Get().Open("dir/qwe.txt");

	//////////

	m_pScriptedObject = new Object();
	Modules::ScriptSystem->CreateScriptComponentOnObject(*m_pScriptedObject, "TestScript");
	ScriptComponent* scriptComp = m_pScriptedObject->GetComponent<ScriptComponent>();
	if(scriptComp != nullptr)
	{
		scriptComp->CallByType(ScriptComponent::EBuiltinFuncType::Init);

		scriptComp->Call("Function1");
		scriptComp->Call("Function2", 12.3f, 100);
		scriptComp->Call("Function3", String("TEST"));
	}

	//////////

	REGISTER_NATIVE_SCRIPT(SimpleRotateScript);

	m_pCubeObj = new Object();
	Modules::ScriptSystem->CreateScriptComponentOnObject(*m_pCubeObj, "SimpleRotateScript");

	//////////

	std::shared_ptr<Mesh> mesh = Modules::Application->GetResourceManager().GetMesh("data/sibenik/sibenik.obj");
	std::shared_ptr<Texture> texture = Modules::Application->GetResourceManager().GetTexture("data/sibenik/mramor6x6.png");

	//////////

	for(const Keyboard& keyboard : Modules::Input->GetKeyboards())
	{
		m_inputMapping.MapButton(EAction::Exit, keyboard, KEY_ESCAPE);
		m_inputMapping.MapButton(EAction::Fire, keyboard, KEY_SPACE);
	}

	for(const Mouse& mouse : Modules::Input->GetMouses())
	{
		m_inputMapping.MapButton(EAction::Fire, mouse, Mouse::Button_Left);
		m_inputMapping.MapAxis(EAction::CameraYaw, mouse, Mouse::Axis_X);
		m_inputMapping.MapAxis(EAction::CameraPitch, mouse, Mouse::Axis_Y);
	}

	for(const GameController& gamepad : Modules::Input->GetControllers())
	{
		m_inputMapping.MapButton(EAction::Fire, gamepad, GameController::BTN_X);
	}

	//////////

	StackString<DESIRE_MAX_PATH_LEN> dataDirPath = FileSystem::Get().GetAppDirectory();
	dataDirPath += "data";

	m_spDataDirWatcher = std::make_unique<FileSystemWatcher>(dataDirPath, [](FileSystemWatcher::EAction action, const String& filename)
	{
		const char* strAction = "";
		switch(action)
		{
			case FileSystemWatcher::EAction::Added:		strAction = "added";  break;
			case FileSystemWatcher::EAction::Deleted:	strAction = "deleted"; break;
			case FileSystemWatcher::EAction::Modified:	strAction = "modified"; break;
		}
		LOG_MESSAGE("%s %s", filename.Str(), strAction);
	});

	//////////


	LOG_DEBUG("Init done");
}

void SandBox::Kill()
{
	delete m_pScriptedObject;
	m_pScriptedObject = nullptr;
}

void SandBox::Update()
{
	if(m_inputMapping.WasPressed(EAction::Exit))
	{
		LOG_MESSAGE("!EXIT!");
		return;
	}

	if(m_inputMapping.IsDown(EAction::Fire))
	{
		LOG_MESSAGE("Fire is down");
	}

	FileSystemWatcher::UpdateAll();

	Modules::UI->NewFrame(*m_spMainWindow);

	// Variables for UI testing
	if(Modules::UI->BeginWindow("Test Window", Vector2(100, 100), Vector2(350, 600)))
	{
		Modules::UI->Text("TextInput:");
		static DynamicString textValue;
		Modules::UI->TextInput(textValue);

		Modules::UI->Button("Button");

		Modules::UI->ArrowButton(UI::EArrowDir::Left);

		static bool isCheckboxChecked = false;
		Modules::UI->Checkbox(isCheckboxChecked, "Checkbox");

		enum class ERadioButtonOption
		{
			A,
			B,
			C
		};
		static ERadioButtonOption radioButtonOption = ERadioButtonOption::A;
		Modules::UI->RadioButton("RadioButton - A", radioButtonOption, ERadioButtonOption::A);
		Modules::UI->RadioButton("RadioButton - B", radioButtonOption, ERadioButtonOption::B);
		Modules::UI->RadioButton("RadioButton - C", radioButtonOption, ERadioButtonOption::C);

		constexpr float ratio[] = { 0.3f, 0.7f };
		Modules::UI->LayoutColumns(2, ratio);

		static float sliderValue = 0.5f;
		Modules::UI->Text("Slider");
		Modules::UI->Slider(sliderValue, 0.0f, 1.0f);

		Modules::UI->Text("ProgressBar");
		Modules::UI->ProgressBar(sliderValue);

		static int32_t spinnerValue = 0;
		Modules::UI->Text("ValueSpinner");
		Modules::UI->ValueSpinner(spinnerValue);

		static float floatValue = 0.0f;
		Modules::UI->Text("ValueEdit");
		Modules::UI->ValueEdit(floatValue);

		static Vector3 vec3 = Vector3::Zero();
		Modules::UI->Text("ValueEdit");
		Modules::UI->ValueEdit(vec3);

		static 	float color[4] = { 0.0f, 1.0f, 0.0f, 1.0f };
		Modules::UI->Text("ColorPicker");
		Modules::UI->ColorPicker(color);
	}
	Modules::UI->EndWindow();

	// Render
	Modules::Render->BeginFrame(*m_spMainWindow);

	Modules::UI->Render();

	Modules::Render->EndFrame();
}
