project "Script-Squirrel"
	AddModuleConfig()
	uuid "05885A51-71F1-4606-A989-2AA3C69EEA91"

	pchheader "stdafx_Squirrel.h"
	pchsource "../src/stdafx_Squirrel.cpp"

	defines
	{
		"SQ_EXCLUDE_DEFAULT_MEMFUNCTIONS",
		"SCRAT_USE_CXX11_OPTIMIZATIONS",
	}

	includedirs
	{
		"../Externals/squirrel/include",
		"../Externals/sqrat/include",
	}

	files
	{
		"../Externals/squirrel/include/sqconfig.h",
		"../Externals/squirrel/include/squirrel.h",
		"../Externals/squirrel/squirrel/*.h",
		"../Externals/squirrel/squirrel/*.cpp",
		"../Externals/sqrat/include/**.h",
	}
