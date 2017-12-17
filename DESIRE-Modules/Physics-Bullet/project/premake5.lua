project "Physics-Bullet"
	AddModuleConfig()
	uuid "F4BCB217-F30B-4DE4-9F3C-F1B2013FA45C"

	includedirs
	{
		"../Externals/Bullet/src",
	}

	files
	{
		"../Externals/**.h",
		"../Externals/**.cpp",
	}
