project "DESIRE"
	kind "StaticLib"
	location(_ACTION)
	targetdir("../lib/" .. _ACTION)
	objdir(_ACTION .. "/obj/%{cfg.buildcfg}/")

	uuid "74923478-6D8D-4892-A013-C2B759A870EC"

	includedirs
	{
		"../src",
	}

	files
	{
		"../src/**.h",
		"../src/**.hpp",
		"../src/**.cpp",
	}

	filter "system:not android"
		removefiles
		{
			"../src/**/ANDROID/**",
		}

	filter "system:not ios"
		removefiles
		{
			"../src/**/IOS/**",
		}

	filter "system:not linux"
		removefiles
		{
			"../src/**/LINUX/**",
		}

	filter "system:not macosx"
		removefiles
		{
			"../src/**/OSX/**",
		}

	filter "system:not windows"
		removefiles
		{
			"../src/**/WINDOWS/**",
		}
