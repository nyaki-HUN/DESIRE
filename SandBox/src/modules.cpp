// --------------------------------------------------------------------------------------------------------------------
//	App
// --------------------------------------------------------------------------------------------------------------------
#include "SandBox.h"
DESIRE_APP_CLASS(SandBox)

// --------------------------------------------------------------------------------------------------------------------
//	Compression
// --------------------------------------------------------------------------------------------------------------------
#include "Engine/Compression/CompressionManager.h"

#include "Compression-zlib-ng/src/GZipCompression.h"
#include "Compression-zlib-ng/src/ZlibCompression.h"
#include "Compression-zlib-ng/src/RawDeflateCompression.h"
#include "Compression-zstd/src/ZstdCompression.h"
const HashedStringMap<CompressionManager::CompressionFactoryFunc_t> CompressionManager::compressionFactories =
{
	{ "gzip", &CompressionFactory<GZipCompression> },
	{ "zlib", &CompressionFactory<ZlibCompression> },
	{ "deflate", &CompressionFactory<RawDeflateCompression> },
	{ "zstd", &CompressionFactory<ZstdCompression> },
};

// --------------------------------------------------------------------------------------------------------------------
//	Physics
// --------------------------------------------------------------------------------------------------------------------
#include "Physics-Box2D/src/Box2DPhysics.h"
#include "Physics-Bullet/src/BulletPhysics.h"
//#include "Physics-PhysX/src/PhysXPhysics.h"
Physics *Physics::instance = new Box2DPhysics();
//Physics *Physics::instance = new BulletPhysics();
//Physics *Physics::instance = new PhysXPhysics();

// --------------------------------------------------------------------------------------------------------------------
//	Render
// --------------------------------------------------------------------------------------------------------------------
#include "Render-bgfx/src/BgfxRender.h"
#include "Render-Direct3D11/src/Direct3D11Render.h"
//Render *Render::instance = new BgfxRender;
Render *Render::instance = new Direct3D11Render;

// --------------------------------------------------------------------------------------------------------------------
//	Resource
// --------------------------------------------------------------------------------------------------------------------
#include "Engine/Resource/ResourceManager.h"

#include "ResourceLoader-Assimp/src/AssimpLoader.h"
const std::vector<ResourceManager::MeshLoaderFunc_t> ResourceManager::meshLoaders =
{
	&AssimpLoader::Load,
};

#include "Engine/Resource/ShaderLoader/FileShaderLoader.h"
const std::vector<ResourceManager::ShaderLoaderFunc_t> ResourceManager::shaderLoaders =
{
	&FileShaderLoader::Load,
};

#include "Engine/Resource/TextureLoader/TgaLoader.h"
#include "Engine/Resource/TextureLoader/StbImageLoader.h"
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
//ScriptSystem *ScriptSystem::instance = new AngelScriptSystem();
//ScriptSystem *ScriptSystem::instance = new LuaScriptSystem();
ScriptSystem *ScriptSystem::instance = new SquirrelScriptSystem();

// --------------------------------------------------------------------------------------------------------------------
//	Sound
// --------------------------------------------------------------------------------------------------------------------
#include "Engine/Sound/Sound.h"
Sound *Sound::instance = nullptr;
