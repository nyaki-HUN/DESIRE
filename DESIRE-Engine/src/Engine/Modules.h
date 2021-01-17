#pragma once

class Application;
class Input;
class Physics;
class Render;
class ScriptSystem;
class SoundSystem;
class UI;

struct Modules
{
	static std::unique_ptr<Application> Application;
	static std::unique_ptr<Input> Input;
	static std::unique_ptr<Physics> Physics;
	static std::unique_ptr<Render> Render;
	static std::unique_ptr<ScriptSystem> ScriptSystem;
	static std::unique_ptr<SoundSystem> SoundSystem;
	static std::unique_ptr<UI> UI;
};
