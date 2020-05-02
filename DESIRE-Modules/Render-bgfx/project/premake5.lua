project "Render-bgfx"
	AddModuleConfig()
	uuid "39620121-0C61-49D9-9BF4-B67638336D45"

	pchheader "stdafx_bgfx.h"
	pchsource "../src/stdafx_bgfx.cpp"

	removeflags { "FatalCompileWarnings" }

	includedirs
	{
		"../Externals/bgfx/include",
		"../Externals/bgfx/3rdparty",
		"../Externals/bgfx/3rdparty/dxsdk/include",
		"../Externals/bgfx/3rdparty/khronos",
		"../Externals/bimg/include",
		"../Externals/bimg/3rdparty/astc-codec",
		"../Externals/bimg/3rdparty/astc-codec/include",
		"../Externals/bx/include",
	}

	files
	{
		"../Externals/**.h",
		"../Externals/**.cpp",
		"../Externals/**.cc",
	}

	removefiles
	{
		"../Externals/**/amalgamated.*",
	}

	filter { "files:../Externals/**.cc" }
		flags { "NoPCH" }

	filter "action:vs*"
		includedirs
		{
			"../Externals/bx/include/compat/msvc",
		}

	filter "Debug"
		defines
		{
			"BX_CONFIG_DEBUG=1",
		}
