project "SandBox"
	AddCommonProjectConfig();
	uuid "97C50FE7-F697-4ABD-8212-7195BB4F410F"
	kind "WindowedApp"
	targetdir("../")

	includedirs
	{
		"../../DESIRE-Engine/src",
		"../../DESIRE-Engine/3rdparty",
		"../../DESIRE-modules",
	}

	links
	{
		"DESIRE",
		"Compression-zlib-ng",
		"Compression-zstd",
		"Physics-Box2D",
		"Physics-Bullet",
		"Render-bgfx",
		"Render-Direct3D11",
		"ResourceLoader-Assimp",
		"Script-AngelScript",
		"Script-Lua",
		"Script-Squirrel",
		"UI-imgui",

		"d3d11",
		"d3dcompiler",
		"dxgi",
		"dxguid",
	}
