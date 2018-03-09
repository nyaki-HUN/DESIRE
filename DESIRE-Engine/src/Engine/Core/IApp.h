#pragma once

#include "Engine/Core/Factory.h"
#include "Engine/Core/IWindow.h"

#include <stdint.h>

class CoreAppEvent;
enum class EAppEventType;
class Physics;
class Render;
class ScriptSystem;
class SoundSystem;

class IApp
{
protected:
	IApp();
	virtual ~IApp();

	static void CreateInstance();

public:
	virtual void Init() = 0;
	virtual void Kill() = 0;
	virtual void Update() = 0;

	// Send application event
	virtual void SendEvent(const CoreAppEvent& event);

	// Send generic application event (without parameters) with 'eventType'
	void SendEvent(EAppEventType eventType);

	static int Start(int argc, const char * const *argv);
	static void Stop(int returnValue = 0);

	inline static IApp* Get()
	{
		return instance;
	}

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

		IWindow::CreationParams windowParams;
		EOrientation defaultOrientation;
		uint8_t supportedOrientations;
	};

protected:
	std::unique_ptr<IWindow> mainWindow;

private:
	void Run();

	virtual CreationParams GetCreationParams(int argc, const char * const *argv);

	void CreateModules();
	static void DestroyModules();

	// Module factories
	static const Factory<Physics>::Func_t physicsFactory;
	static const Factory<Render>::Func_t renderFactory;
	static const Factory<ScriptSystem>::Func_t scriptSystemFactory;
	static const Factory<SoundSystem>::Func_t soundSystemFactory;

	static IApp *instance;
	static bool isMainLoopRunning;
	static int returnValue;
};


#define DESIRE_APP_CLASS(APP_CLASS)		\
	void IApp::CreateInstance()			\
	{									\
		instance = new APP_CLASS();		\
	}
