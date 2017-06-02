#include "stdafx.h"
#include "IApp.h"
#include "Core/platform.h"
#include "Core/CoreAppEvent.h"
#include "Core/Timer.h"
#include "Input/Input.h"
#include "Physics/IPhysics.h"
#include "Render/IRender.h"
#include "Script/IScriptSystem.h"
#include "UI/ImGuiImpl.h"

bool IApp::isMainLoopRunning = false;
int IApp::returnValue = 0;

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

	if(IApp::Get() == nullptr)
	{
		ASSERT(false && "Application instance not found");
		return -1;
	}

	LOG_DEBUG("DESIRE Engine is starting up...");

	// Init engine
	Timer::Get();
	Input::Get();
	IPhysics *physics = IPhysics::Get();
	IScriptSystem *scriptSystem = IScriptSystem::Get();

	SCreationParams params = IApp::Get()->GetCreationParams(argc, argv);
	IWindow *mainWindow = IWindow::Create(params.windowParams);
	IRender::Get()->Init(mainWindow);
	Input::Init(mainWindow);
	ImGuiImpl::Get()->Init();

	// Run App
	IApp::Get()->Init(mainWindow);
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

		IApp::Get()->Update();
	}
	IApp::Get()->Kill();

	// DeInit engine
	ImGuiImpl::Get()->Kill();
	Input::Kill();
	IRender::Get()->Kill();
	delete mainWindow;

	return returnValue;
}

void IApp::Stop(int i_returnValue)
{
	returnValue = i_returnValue;
	isMainLoopRunning = false;
}

IApp::SCreationParams IApp::GetCreationParams(int argc, const char * const *argv)
{
	DESIRE_UNUSED(argc);
	DESIRE_UNUSED(argv);
	return SCreationParams();
}
