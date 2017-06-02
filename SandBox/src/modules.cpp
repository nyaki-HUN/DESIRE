// --------------------------------------------------------------------------------------------------------------------
//	App
// --------------------------------------------------------------------------------------------------------------------
#include "SandBox.h"
DESIRE_DEFINE_SINGLETON_INSTANCE(IApp, SandBox)

// --------------------------------------------------------------------------------------------------------------------
//	Physics
// --------------------------------------------------------------------------------------------------------------------
#include "Physics/Bullet/BulletPhysics.h"
#include "Physics/PhysX/PhysXPhysics.h"
DESIRE_DEFINE_SINGLETON_INSTANCE(IPhysics, BulletPhysics)
//DESIRE_DEFINE_SINGLETON_INSTANCE(IPhysics, PhysXPhysics)

// --------------------------------------------------------------------------------------------------------------------
//	Render
// --------------------------------------------------------------------------------------------------------------------
#include "Render/bgfx/RenderBgfx.h"
DESIRE_DEFINE_SINGLETON_INSTANCE(IRender, RenderBgfx)

// --------------------------------------------------------------------------------------------------------------------
//	Resource
// --------------------------------------------------------------------------------------------------------------------
#include "Resource/ResourceManager.h"

#include "Resource/MeshLoader/AssimpLoader.h"
const std::vector<ResourceManager::MeshLoaderFunc_t> ResourceManager::meshLoaders =
{
	&AssimpLoader::Load,
};

#include "Resource/TextureLoader/TgaLoader.h"
#include "Resource/TextureLoader/StbImageLoader.h"
const std::vector<ResourceManager::TextureLoaderFunc_t> ResourceManager::textureLoaders =
{
	&TgaLoader::Load,
	&StbImageLoader::Load,
};

// --------------------------------------------------------------------------------------------------------------------
//	Script System
// --------------------------------------------------------------------------------------------------------------------
#include "Script/AngelScript/AngelScriptSystem.h"
#include "Script/Lua/LuaScriptSystem.h"
#include "Script/Squirrel/SquirrelScriptSystem.h"
//DESIRE_DEFINE_SINGLETON_INSTANCE(IScriptSystem, AngelScriptSystem)
//DESIRE_DEFINE_SINGLETON_INSTANCE(IScriptSystem, LuaScriptSystem)
DESIRE_DEFINE_SINGLETON_INSTANCE(IScriptSystem, SquirrelScriptSystem)

// --------------------------------------------------------------------------------------------------------------------
//	Sound
// --------------------------------------------------------------------------------------------------------------------
#include "Sound/OpenAL/OpenALSound.h"
DESIRE_DEFINE_SINGLETON_INSTANCE(ISound, OpenALSound)
