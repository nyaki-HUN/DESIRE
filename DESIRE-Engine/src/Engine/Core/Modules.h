#pragma once

#include <memory>

class Input;
class Physics;
class Render;
class ResourceManager;
class ScriptSystem;
class SoundSystem;

class Modules
{
public:
	static std::unique_ptr<Input> Input;
	static std::unique_ptr<Physics> Physics;
	static std::unique_ptr<Render> Render;
	static std::unique_ptr<ResourceManager> ResourceManager;
	static std::unique_ptr<ScriptSystem> ScriptSystem;
	static std::unique_ptr<SoundSystem> SoundSystem;
};
