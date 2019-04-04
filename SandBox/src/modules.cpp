// --------------------------------------------------------------------------------------------------------------------
//	Application
// --------------------------------------------------------------------------------------------------------------------
#include "SandBox.h"
const Factory<Application>::Func_t Application::applicationFactory =
	&Factory<Application>::Create<SandBox>;

// --------------------------------------------------------------------------------------------------------------------
//	Compression
// --------------------------------------------------------------------------------------------------------------------
#include "Engine/Compression/CompressionManager.h"

#include "Compression-zlib-ng/src/GZipCompression.h"
#include "Compression-zlib-ng/src/ZlibCompression.h"
#include "Compression-zlib-ng/src/RawDeflateCompression.h"
#include "Compression-zstd/src/ZstdCompression.h"
const HashedStringMap<Factory<Compression>::Func_t> CompressionManager::compressionFactories =
{
	{ "gzip", &Factory<Compression>::Create<GZipCompression> },
	{ "zlib", &Factory<Compression>::Create<ZlibCompression> },
	{ "deflate", &Factory<Compression>::Create<RawDeflateCompression> },
	{ "zstd", &Factory<Compression>::Create<ZstdCompression> },
};

// --------------------------------------------------------------------------------------------------------------------
//	Physics
// --------------------------------------------------------------------------------------------------------------------
#include "Physics-Box2D/src/Box2DPhysics.h"
#include "Physics-Bullet/src/BulletPhysics.h"
//#include "Physics-PhysX/src/PhysXPhysics.h"
const Factory<Physics>::Func_t Application::physicsFactory =
	&Factory<Physics>::Create<Box2DPhysics>;
//	&Factory<Physics>::Create<BulletPhysics>;
//	&Factory<Physics>::Create<PhysXPhysics>;

// --------------------------------------------------------------------------------------------------------------------
//	Render
// --------------------------------------------------------------------------------------------------------------------
#include "Render-bgfx/src/BgfxRender.h"
#include "Render-Direct3D11/src/Direct3D11Render.h"
#include "Render-Direct3D12/src/Direct3D12Render.h"
const Factory<Render>::Func_t Application::renderFactory =
//	&Factory<Render>::Create<BgfxRender>;
//	&Factory<Render>::Create<Direct3D11Render>;
	&Factory<Render>::Create<Direct3D12Render>;

// --------------------------------------------------------------------------------------------------------------------
//	Resource
// --------------------------------------------------------------------------------------------------------------------
#include "Engine/Resource/ResourceManager.h"

#include "ResourceLoader-Assimp/src/AssimpLoader.h"
const Array<ResourceManager::MeshLoaderFunc_t> ResourceManager::meshLoaders =
{
	&AssimpLoader::Load,
};

#include "Engine/Resource/ShaderLoader/FileShaderLoader.h"
const Array<ResourceManager::ShaderLoaderFunc_t> ResourceManager::shaderLoaders =
{
	&FileShaderLoader::Load,
};

#include "Engine/Resource/TextureLoader/TgaLoader.h"
#include "Engine/Resource/TextureLoader/StbImageLoader.h"
const Array<ResourceManager::TextureLoaderFunc_t> ResourceManager::textureLoaders =
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
const Factory<ScriptSystem>::Func_t Application::scriptSystemFactory =
//	&Factory<ScriptSystem>::Create<AngelScriptSystem>;
//	&Factory<ScriptSystem>::Create<LuaScriptSystem>;
	&Factory<ScriptSystem>::Create<SquirrelScriptSystem>;

// --------------------------------------------------------------------------------------------------------------------
//	Sound
// --------------------------------------------------------------------------------------------------------------------
const Factory<SoundSystem>::Func_t Application::soundSystemFactory = nullptr;
