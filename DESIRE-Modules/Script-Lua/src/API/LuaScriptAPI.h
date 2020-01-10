#pragma once

#include "Engine/Modules.h"

#include "sol/sol.hpp"

// Engine API register functions
void RegisterCoreAPI_Math_Lua(sol::state_view& lua);
void RegisterCoreAPI_Lua(sol::state_view& lua);
void RegisterInputAPI_Lua(sol::state_view& lua);
void RegisterPhysicsAPI_Lua(sol::state_view& lua);
void RegisterRenderAPI_Lua(sol::state_view& lua);
void RegisterSoundAPI_Lua(sol::state_view& lua);
