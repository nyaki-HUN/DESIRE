project "Compression-Lizard"
	AddModuleConfig()
	uuid "BC2B4D7F-87C2-4E42-9DBD-EDEAEBEC1BE0"

	pchheader "stdafx_Lizard.h"
	pchsource "../src/stdafx_Lizard.cpp"

	includedirs
	{
		"../Externals/Lizard/include",
	}

	files
	{
		"../Externals/**.h",
		"../Externals/**.c",
	}
