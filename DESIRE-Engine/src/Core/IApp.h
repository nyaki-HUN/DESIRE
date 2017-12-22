#pragma once

#include "Core/Singleton.h"
#include "Core/IWindow.h"

#include <stdint.h>

class CoreAppEvent;
enum class EAppEventType;

class IApp
{
	DESIRE_DECLARE_SINGLETON_INTERFACE(IApp)
	static void CreateInstance();

public:
	virtual void Init(IWindow *mainWindow) = 0;
	virtual void Kill() = 0;
	virtual void Update() = 0;

	// Send application event
	virtual void SendEvent(const CoreAppEvent& event);

	// Send generic application event (without parameters) with 'eventType'
	void SendEvent(EAppEventType eventType);

	static int Run(int argc, const char * const *argv);
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

		IWindow::CreationParams windowParams;
		EOrientation defaultOrientation;
		uint8_t supportedOrientations;
	};

private:
	virtual CreationParams GetCreationParams(int argc, const char * const *argv);

	static IApp* instance;
	static bool isMainLoopRunning;
	static int returnValue;
};


#define DESIRE_APP_CLASS(APP_CLASS)		\
	void IApp::CreateInstance()			\
	{									\
		instance = new APP_CLASS();		\
	}
