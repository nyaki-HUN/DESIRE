project "Physics-PhysX"
	AddModuleConfig()
	uuid "A7BB4030-6C2C-45AB-9C46-F1C3A6ACFB0F"

	pchheader "stdafx_PhysX.h"
	pchsource "../src/stdafx_PhysX.cpp"

	includedirs
	{
		"../Externals/PhysX_3.4/Include",
		"../Externals/PxShared/include",
	}

	files
	{
		"../Externals/**.h",
	}
