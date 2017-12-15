solution "Script-Squirrel"
	location(_ACTION)

	project "Script-Squirrel"
		AddModuleConfig()
		uuid "05885A51-71F1-4606-A989-2AA3C69EEA91"

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
