project "Physics-Box2D"
	AddModuleConfig()
	uuid "051565BD-D2E6-46B0-851D-A3B5B3AE3BDB"

	pchheader "stdafx_Box2D.h"
	pchsource "../src/stdafx_Box2D.cpp"

	includedirs
	{
		"../Externals/Box2D/include",
		"../Externals/Box2D/src",
	}

	files
	{
		"../Externals/Box2D/**.h",
		"../Externals/Box2D/**.cpp",
	}
