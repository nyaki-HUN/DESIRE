project "Compression-LZ4"
	AddModuleConfig()
	uuid "83295281-AF7D-4CBD-B089-92DFE9016E28"

	pchheader "stdafx_LZ4.h"
	pchsource "../src/stdafx_LZ4.cpp"

	includedirs
	{
		"../Externals/LZ4/include",
	}

	files
	{
		"../Externals/**.h",
		"../Externals/**.c",
	}
