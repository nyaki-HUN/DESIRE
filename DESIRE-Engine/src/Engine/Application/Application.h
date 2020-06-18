#pragma once

#include "Engine/Application/OSWindowCreationParams.h"
#include "Engine/Core/Factory.h"

class CoreAppEvent;
class OSWindow;
class Physics;
class Render;
class ScriptSystem;
class SoundSystem;
class Timer;
class UI;
enum class EAppEventType;

class Application
{
protected:
	Application();

public:
	virtual ~Application();

	const Timer* GetTimer() const;

	// Send application event
	virtual void SendEvent(const CoreAppEvent& event);

	// Send generic application event (without parameters) with 'eventType'
	void SendEvent(EAppEventType eventType);

	virtual void Init() = 0;
	virtual void Kill() = 0;
	virtual void Update() = 0;

	static int Start(int argc, const char* const* argv);
	static void Stop(int returnValue = 0);

protected:
	struct CreationParams
	{
		OSWindowCreationParams windowParams;
	};

	std::unique_ptr<Timer> timer;
	std::unique_ptr<OSWindow> mainWindow;

private:
	void Run();

	virtual CreationParams GetCreationParams(int argc, const char* const* argv);

	static void CreateModules();
	static void DestroyModules();

	// Module factories
	static const Factory<Application>::Func_t s_applicationFactory;
	static const Factory<Physics>::Func_t s_physicsFactory;
	static const Factory<Render>::Func_t s_renderFactory;
	static const Factory<ScriptSystem>::Func_t s_scriptSystemFactory;
	static const Factory<SoundSystem>::Func_t s_soundSystemFactory;
	static const Factory<UI>::Func_t s_uiFactory;

	static bool s_isMainLoopRunning;
	static int s_returnValue;
};
