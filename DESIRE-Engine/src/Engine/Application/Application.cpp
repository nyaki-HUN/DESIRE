#include "Engine/stdafx.h"
#include "Engine/Application/Application.h"
#include "Engine/Application/CoreAppEvent.h"
#include "Engine/Core/Modules.h"
#include "Engine/Core/Timer.h"
#include "Engine/Input/Input.h"
#include "Engine/Physics/Physics.h"
#include "Engine/Render/Render.h"
#include "Engine/Resource/ResourceManager.h"
#include "Engine/Script/ScriptSystem.h"
#include "Engine/Sound/SoundSystem.h"

bool Application::isMainLoopRunning = false;
int Application::returnValue = 0;

Application::Application()
{
	timer = std::make_unique<Timer>();
}

Application::~Application()
{

}

const Timer* Application::GetTimer() const
{
	return timer.get();
}

void Application::SendEvent(const CoreAppEvent& event)
{
	if(event.eventType == EAppEventType::EnterBackground)
	{
		Modules::Input->Reset();
	}
}

void Application::SendEvent(EAppEventType eventType)
{
	ASSERT(eventType != EAppEventType::Notification && "Use NotificationEvent class");
	ASSERT(eventType != EAppEventType::KeyboardWillShow && "Use KeyboardWillShowEvent class");

	SendEvent(CoreAppEvent(eventType));
}

int Application::Start(int argc, const char * const *argv)
{
	ASSERT(!isMainLoopRunning);

	if(applicationFactory == nullptr)
	{
		ASSERT(false && "Application factory is not set");
		return -1;
	}

	LOG_DEBUG("DESIRE Engine is starting up...");
	Modules::Application = applicationFactory();
	CreateModules();

	// Create main window
	{
		CreationParams params = Modules::Application->GetCreationParams(argc, argv);
		Modules::Application->mainWindow = OSWindow::Create(params.windowParams);
	}

	Modules::Render->Init(Modules::Application->mainWindow.get());
	Modules::Input->Init(Modules::Application->mainWindow.get());

	Modules::Application->Run();

	Modules::Input->Kill();
	Modules::Render->Kill();

	Modules::Application = nullptr;
	DestroyModules();

	return returnValue;
}

void Application::Stop(int i_returnValue)
{
	returnValue = i_returnValue;
	isMainLoopRunning = false;
}

void Application::Run()
{
	Init();

	isMainLoopRunning = true;
	while(isMainLoopRunning)
	{
		timer->Update();
		Modules::Input->Update();

		mainWindow->HandleWindowMessages();

		if(Modules::ScriptSystem != nullptr)
		{
			Modules::ScriptSystem->Update();
		}

		if(Modules::Physics != nullptr)
		{
			const float deltaTime = timer->GetSecDelta();
			Modules::Physics->Update(deltaTime);
		}

		Update();
	}

	Kill();
}

Application::CreationParams Application::GetCreationParams(int argc, const char * const *argv)
{
	DESIRE_UNUSED(argc);
	DESIRE_UNUSED(argv);
	return CreationParams();
}

void Application::CreateModules()
{
	Modules::Input = std::make_unique<Input>();

	if(physicsFactory != nullptr)
	{
		Modules::Physics = physicsFactory();
	}

	if(renderFactory != nullptr)
	{
		Modules::Render = renderFactory();
	}

	Modules::ResourceManager = std::make_unique<ResourceManager>();

	if(soundSystemFactory != nullptr)
	{
		Modules::SoundSystem = soundSystemFactory();
	}

	if(scriptSystemFactory != nullptr)
	{
		Modules::ScriptSystem = scriptSystemFactory();
	}
}

void Application::DestroyModules()
{
	Modules::ScriptSystem = nullptr;
	Modules::SoundSystem = nullptr;
	Modules::ResourceManager = nullptr;
	Modules::Render = nullptr;
	Modules::Physics = nullptr;
	Modules::Input = nullptr;
}
