project "Compression-zlib"
	AddModuleConfig()
	uuid "07C8AFBA-5E54-4A5B-BD68-C5C8ED113D57"

	pchheader "stdafx_zlib.h"
	pchsource "../src/stdafx_zlib.cpp"

	defines
	{
		"ZLIB_CONST",
	}

	includedirs
	{
		"../Externals/zlib",
	}

	files
	{
		"../Externals/**.h",
		"../Externals/**.c",
	}

	removefiles
	{
		"../Externals/zlib/gz*.h",
		"../Externals/zlib/gz*.c",
	}

	filter "action:linux"
		defines
		{
			"LUA_USE_LINUX",
		}
