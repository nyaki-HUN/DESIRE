#include "Engine/stdafx.h"
#include "Engine/Core/IApp.h"
#include "Engine/Core/CoreAppEvent.h"
#include "Engine/Core/Modules.h"
#include "Engine/Core/Timer.h"
#include "Engine/Input/Input.h"
#include "Engine/Physics/Physics.h"
#include "Engine/Render/Render.h"
#include "Engine/Script/ScriptSystem.h"
#include "Engine/Sound/SoundSystem.h"

IApp *IApp::instance = nullptr;
bool IApp::isMainLoopRunning = false;
int IApp::returnValue = 0;

IApp::IApp()
{

}

IApp::~IApp()
{

}

void IApp::SendEvent(const CoreAppEvent& event)
{
	DESIRE_UNUSED(event);
}

void IApp::SendEvent(EAppEventType eventType)
{
	ASSERT(eventType != EAppEventType::NOTIFICATION && "Use NotificationEvent class");
	ASSERT(eventType != EAppEventType::KEYBOARD_WILL_SHOW && "Use KeyboardWillShowEvent class");

	SendEvent(CoreAppEvent(eventType));
}

int IApp::Start(int argc, const char * const *argv)
{
	ASSERT(!isMainLoopRunning);

	CreateInstance();
	if(instance == nullptr)
	{
		ASSERT(false && "Application instance not found");
		return -1;
	}

	LOG_DEBUG("DESIRE Engine is starting up...");

	instance->CreateModules();

	CreationParams params = instance->GetCreationParams(argc, argv);
	instance->mainWindow = IWindow::Create(params.windowParams);

	instance->Run();

	delete instance;
	instance = nullptr;

	DestroyModules();

	return returnValue;
}

void IApp::Stop(int i_returnValue)
{
	returnValue = i_returnValue;
	isMainLoopRunning = false;
}

void IApp::Run()
{
	Modules::Render->Init(mainWindow.get());
	Input::Init(mainWindow.get());

	Init();

	isMainLoopRunning = true;
	while(isMainLoopRunning)
	{
		Timer::Get()->Update();
		Input::Get()->Update();

		mainWindow->HandleWindowMessages();

		if(Modules::ScriptSystem != nullptr)
		{
			Modules::ScriptSystem->Update();
		}

		if(Modules::Physics != nullptr)
		{
			Modules::Physics->Update();
		}

		Update();
	}

	Kill();

	Input::Kill();
	Modules::Render->Kill();
}

IApp::CreationParams IApp::GetCreationParams(int argc, const char * const *argv)
{
	DESIRE_UNUSED(argc);
	DESIRE_UNUSED(argv);
	return CreationParams();
}

void IApp::CreateModules()
{
	if(physicsFactory != nullptr)
	{
		Modules::Physics = physicsFactory();
	}

	if(renderFactory != nullptr)
	{
		Modules::Render = renderFactory();
	}
	
	if(scriptSystemFactory != nullptr)
	{
		Modules::ScriptSystem = scriptSystemFactory();
	}

	if(soundSystemFactory != nullptr)
	{
		Modules::SoundSystem = soundSystemFactory();
	}
}

void IApp::DestroyModules()
{
	Modules::Physics = nullptr;
	Modules::Render = nullptr;
	Modules::ScriptSystem = nullptr;
}
