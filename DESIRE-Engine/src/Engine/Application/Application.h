#pragma once

#include "Engine/Application/OSWindowCreationParams.h"
#include "Engine/Core/Factory.h"

class CoreAppEvent;
class OSWindow;
class Physics;
class Render;
class ResourceManager;
class ScriptSystem;
class SoundSystem;
class Timer;
class UI;

enum class EAppEventType
{
	Activate,
	Deactivate
};

class Application
{
protected:
	Application();

public:
	virtual ~Application();

	const Timer* GetTimer() const;
	ResourceManager& GetResourceManager();

	virtual void SendEvent(EAppEventType eventType, const void* pUserData = nullptr);

	virtual void Init() = 0;
	virtual void Kill() = 0;
	virtual void Update() = 0;

	static int Start(int argc, const char* const* ppArgv);
	static void Stop(int returnValue = 0);

protected:
	struct CreationParams
	{
		OSWindowCreationParams windowParams;
	};

	std::unique_ptr<Timer> m_spTimer;
	std::unique_ptr<ResourceManager> m_spResourceManager;
	std::unique_ptr<OSWindow> m_spMainWindow;

private:
	void Run();

	virtual CreationParams GetCreationParams(int argc, const char* const* ppArgv);

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
