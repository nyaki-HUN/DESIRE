project "SandBox"
	kind "WindowedApp"
	location(_ACTION)
	objdir(_ACTION .. "/obj/%{cfg.buildcfg}/")
	targetdir("../")

	uuid "97C50FE7-F697-4ABD-8212-7195BB4F410F"

	includedirs
	{
		"../src",
		"../../DESIRE-Engine/src",
		"../../../DESIRE-modules",
	}

	files
	{
		"../src/**.h",
		"../src/**.cpp",
	}

	configuration "Debug"
		symbols "FastLink"
