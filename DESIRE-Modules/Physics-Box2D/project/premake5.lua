project "Physics-Box2D"
	AddModuleConfig()
	uuid "051565BD-D2E6-46B0-851D-A3B5B3AE3BDB"

	includedirs
	{
		"../Externals/Box2D",
	}

	files
	{
		"../Externals/Box2D/**.h",
		"../Externals/Box2D/**.cpp",
	}
