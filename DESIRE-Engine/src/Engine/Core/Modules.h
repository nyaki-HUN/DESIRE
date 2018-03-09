#pragma once

#include <memory>

class Physics;
class Render;
class ScriptSystem;
class SoundSystem;

class Modules
{
public:
	static std::unique_ptr<Physics> Physics;
	static std::unique_ptr<Render> Render;
	static std::unique_ptr<ScriptSystem> ScriptSystem;
	static std::unique_ptr<SoundSystem> SoundSystem;
};
