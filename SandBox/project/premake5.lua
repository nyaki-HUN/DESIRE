project "SandBox"
	AddCommonProjectConfig();
	uuid "97C50FE7-F697-4ABD-8212-7195BB4F410F"
	kind "WindowedApp"
	targetdir("../")

	pchheader "stdafx.h"
	pchsource "../src/stdafx.cpp"

	includedirs
	{
		"../../DESIRE-Engine/src",
		"../../DESIRE-Engine/3rdparty",
		"../../DESIRE-modules",
	}

	links
	{
		"DESIRE",
		"Compression-Lizard",
		"Compression-LZ4",
		"Compression-zlib",
		"Compression-zstd",
		"Network-curl",
		"Physics-Box2D",
		"Physics-Bullet",
		"Physics-PhysX",
		"Render-bgfx",
		"Render-Direct3D11",
		"Render-Direct3D12",
		"ResourceLoader-Assimp",
		"Script-AngelScript",
		"Script-Lua",
		"Script-Squirrel",
		"UI-imgui",

		"d3d11",
		"d3d12",
		"d3dcompiler",
		"dxgi",
		"dxguid",
	}
