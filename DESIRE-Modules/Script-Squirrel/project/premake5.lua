project "Script-Squirrel"
	AddModuleConfig()
	uuid "05885A51-71F1-4606-A989-2AA3C69EEA91"

	defines { "SCRAT_USE_CXX11_OPTIMIZATIONS" }

	includedirs
	{
		"../Externals/squirrel/include",
		"../Externals/sqrat/include",
	}

	files
	{
		"../Externals/squirrel/include/sqconfig.h",
		"../Externals/squirrel/include/squirrel.h",
		"../Externals/squirrel/squirrel/**.h",
		"../Externals/squirrel/squirrel/**.cpp",
		"../Externals/sqrat/**.h",
	}
