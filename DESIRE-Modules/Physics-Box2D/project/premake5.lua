project "Physics-Box2D"
	AddModuleConfig()
	uuid "A7BB4030-6C2C-45AB-9C46-F1C3A6ACFB0F"
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
