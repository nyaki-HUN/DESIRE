project "DESIRE"
	AddCommonLibConfig()
	warnings "Extra"
	uuid "74923478-6D8D-4892-A013-C2B759A870EC"

	pchheader "stdafx.h"
	pchsource "../src/stdafx.cpp"

	includedirs
	{
		"../3rdparty",
		"../../../DESIRE-modules",
		"../../../DESIRE-modules/Physics-Bullet/src",
		"../../../DESIRE-modules/Render-bgfx/include",
		"../../../DESIRE-modules/ResourceLoader-Assimp/include",
	}

	files
	{
		"../3rdparty/**.h",
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
