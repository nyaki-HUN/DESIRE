// --------------------------------------------------------------------------------------------------------------------
//	App
// --------------------------------------------------------------------------------------------------------------------
#include "SandBox.h"
DESIRE_APP_CLASS(SandBox)

// --------------------------------------------------------------------------------------------------------------------
//	Compression
// --------------------------------------------------------------------------------------------------------------------
#include "Compression/CompressionManager.h"

#include "Compression-zlib-ng/src/ZlibNgCompression.h"
#include "Compression-zstd/src/ZstdCompression.h"
const HashedStringMap<CompressionManager::CompressionFactoryFunc_t> CompressionManager::compressionFactories =
{
	{ "zlib", &CompressionFactory<ZlibNgCompression> },
	{ "zstd", &CompressionFactory<ZstdCompression> },
};

// --------------------------------------------------------------------------------------------------------------------
//	Physics
// --------------------------------------------------------------------------------------------------------------------
#include "Physics-Box2D/src/Box2DPhysics.h"
#include "Physics-Bullet/src/BulletPhysics.h"
//#include "Physics-PhysX/src/PhysXPhysics.h"
DESIRE_DEFINE_SINGLETON_INSTANCE(IPhysics, Box2DPhysics)
//DESIRE_DEFINE_SINGLETON_INSTANCE(IPhysics, BulletPhysics)
//DESIRE_DEFINE_SINGLETON_INSTANCE(IPhysics, PhysXPhysics)

// --------------------------------------------------------------------------------------------------------------------
//	Render
// --------------------------------------------------------------------------------------------------------------------
#include "Render-bgfx/src/BgfxRender.h"
//#include "Render-Direct3D11/src/Direct3D11Render.h"
DESIRE_DEFINE_SINGLETON_INSTANCE(IRender, BgfxRender)
//DESIRE_DEFINE_SINGLETON_INSTANCE(IRender, Direct3D11Render)

// --------------------------------------------------------------------------------------------------------------------
//	Resource
// --------------------------------------------------------------------------------------------------------------------
#include "Resource/ResourceManager.h"

#include "ResourceLoader-Assimp/src/AssimpLoader.h"
const std::vector<ResourceManager::MeshLoaderFunc_t> ResourceManager::meshLoaders =
{
	&AssimpLoader::Load,
};

#include "Resource/ShaderLoader/FileShaderLoader.h"
const std::vector<ResourceManager::ShaderLoaderFunc_t> ResourceManager::shaderLoaders =
{
	&FileShaderLoader::Load,
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
#include "Script-AngelScript/src/AngelScriptSystem.h"
#include "Script-Lua/src/LuaScriptSystem.h"
#include "Script-Squirrel/src/SquirrelScriptSystem.h"
//DESIRE_DEFINE_SINGLETON_INSTANCE(IScriptSystem, AngelScriptSystem)
//DESIRE_DEFINE_SINGLETON_INSTANCE(IScriptSystem, LuaScriptSystem)
DESIRE_DEFINE_SINGLETON_INSTANCE(IScriptSystem, SquirrelScriptSystem)

// --------------------------------------------------------------------------------------------------------------------
//	Sound
// --------------------------------------------------------------------------------------------------------------------
#include "Sound/ISound.h"
DESIRE_DEFINE_EMPTY_SINGLETON_INSTANCE(ISound)
