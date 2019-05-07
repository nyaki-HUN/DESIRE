#pragma once

#include "Engine/Application/OSWindow.h"
#include "Engine/Core/Factory.h"

#include <stdint.h>

class CoreAppEvent;
enum class EAppEventType;
class Timer;
class Physics;
class Render;
class ScriptSystem;
class SoundSystem;

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

	static int Start(int argc, const char * const *argv);
	static void Stop(int returnValue = 0);

protected:
	enum EOrientation
	{
		ORIENTATION_PORTRAIT			= 0x01,
		ORIENTATION_PORTRAIT_REVERSE	= 0x02,
		ORIENTATION_LANDSCAPE_LEFT		= 0x04,
		ORIENTATION_LANDSCAPE_RIGHT		= 0x08,
		ORIENTATION_ALL					= ORIENTATION_PORTRAIT | ORIENTATION_PORTRAIT_REVERSE | ORIENTATION_LANDSCAPE_LEFT | ORIENTATION_LANDSCAPE_RIGHT
	};

	struct CreationParams
	{
		CreationParams()
			: defaultOrientation(ORIENTATION_PORTRAIT)
			, supportedOrientations(ORIENTATION_ALL)
		{

		}

		OSWindow::CreationParams windowParams;
		EOrientation defaultOrientation;
		uint8_t supportedOrientations;
	};

	std::unique_ptr<Timer> timer;
	std::unique_ptr<OSWindow> mainWindow;

private:
	void Run();

	virtual CreationParams GetCreationParams(int argc, const char * const *argv);

	static void CreateModules();
	static void DestroyModules();

	// Module factories
	static const Factory<Application>::Func_t s_applicationFactory;
	static const Factory<Physics>::Func_t s_physicsFactory;
	static const Factory<Render>::Func_t s_renderFactory;
	static const Factory<ScriptSystem>::Func_t s_scriptSystemFactory;
	static const Factory<SoundSystem>::Func_t s_soundSystemFactory;

	static bool s_isMainLoopRunning;
	static int s_returnValue;
};
