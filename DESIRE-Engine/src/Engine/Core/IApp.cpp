#include "Engine/stdafx.h"
#include "Engine/Core/IApp.h"
#include "Engine/Core/CoreAppEvent.h"
#include "Engine/Core/Timer.h"
#include "Engine/Input/Input.h"
#include "Engine/Physics/Physics.h"
#include "Engine/Render/Render.h"
#include "Engine/Script/ScriptSystem.h"

IApp* IApp::instance = nullptr;
bool IApp::isMainLoopRunning = false;
int IApp::returnValue = 0;

IApp::IApp()
{

}

IApp::~IApp()
{

}

IApp* IApp::Get()
{
	return instance;
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

int IApp::Run(int argc, const char * const *argv)
{
	ASSERT(!isMainLoopRunning);

	CreateInstance();
	if(instance == nullptr)
	{
		ASSERT(false && "Application instance not found");
		return -1;
	}

	LOG_DEBUG("DESIRE Engine is starting up...");

	// Init engine
	Timer::Get();
	Input::Get();
	Physics *physics = Physics::Get();
	ScriptSystem *scriptSystem = ScriptSystem::Get();

	CreationParams params = IApp::Get()->GetCreationParams(argc, argv);
	IWindow *mainWindow = IWindow::Create(params.windowParams);
	Render::Get()->Init(mainWindow);
	Input::Init(mainWindow);

	// Run App
	instance->Init(mainWindow);
	isMainLoopRunning = true;
	while(isMainLoopRunning)
	{
		Timer::Get()->Update();
		Input::Get()->Update();

		mainWindow->HandleWindowMessages();

		if(scriptSystem != nullptr)
		{
			scriptSystem->Update();
		}

		if(physics != nullptr)
		{
			physics->Update();
		}

		instance->Update();
	}
	instance->Kill();
	delete instance;
	instance = nullptr;

	// DeInit engine
	Input::Kill();
	Render::Get()->Kill();
	delete mainWindow;

	return returnValue;
}

void IApp::Stop(int i_returnValue)
{
	returnValue = i_returnValue;
	isMainLoopRunning = false;
}

IApp::CreationParams IApp::GetCreationParams(int argc, const char * const *argv)
{
	DESIRE_UNUSED(argc);
	DESIRE_UNUSED(argv);
	return CreationParams();
}
