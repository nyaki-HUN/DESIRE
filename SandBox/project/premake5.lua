project "SandBox"
	AddCommonProjectConfig();
	uuid "97C50FE7-F697-4ABD-8212-7195BB4F410F"
	kind "WindowedApp"
	targetdir("../")

	flags { "WinMain" }

	includedirs
	{
		"../../DESIRE-Engine/src",
		"../../DESIRE-Engine/3rdparty",
		"../../DESIRE-modules",
	}

	libdirs
	{
		"../../DESIRE-modules/Compression-zlib-ng/lib/" .. _ACTION,
		"../../DESIRE-modules/Physics-Bullet/lib/" .. _ACTION,
		"../../DESIRE-modules/Physics-PhysX/lib/" .. _ACTION,
		"../../DESIRE-modules/Render-bgfx/lib/" .. _ACTION,
		"../../DESIRE-modules/ResourceLoader-Assimp/lib/".. _ACTION,
		"../../DESIRE-modules/Script-AngelScript/lib/" .. _ACTION,
		"../../DESIRE-modules/Script-Lua/lib/" .. _ACTION,
		"../../DESIRE-modules/Script-Squirrel/lib/" .. _ACTION,
		"../../DESIRE-modules/UI-imgui/lib/" .. _ACTION,
	}

	links
	{
		"DESIRE",
		"d3d11",
		"d3dcompiler",
		"dxgi",
		"dxguid",
	}

	configuration "Debug"
		links
		{
			"Compression-zlib-ng_d",
			"Physics-Bullet_d",
			"Render-bgfx_d",
			"ResourceLoader-Assimp_d",
			"Script-AngelScript_d",
			"Script-Lua_d",
			"Script-Squirrel_d",
			"UI-imgui_d",
		}

	configuration "Release"
		links
		{
			"Compression-zlib-ng",
			"Physics-Bullet",
			"ResourceLoader-Assimp",
			"Render-bgfx",
			"Script-AngelScript",
			"Script-Lua",
			"Script-Squirrel",
			"UI-imgui",
		}
