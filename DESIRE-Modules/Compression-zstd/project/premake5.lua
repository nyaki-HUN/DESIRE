project "Compression-zstd"
	AddModuleConfig()
	uuid "BB7C69B1-33CC-4FF8-8CC8-D8A46A07A3FF"

	pchheader "stdafx_zstd.h"
	pchsource "../src/stdafx_zstd.cpp"

	includedirs
	{
		"../Externals/zstd/include",
		"../Externals/zstd/src/common",
	}

	files
	{
		"../Externals/**.h",
		"../Externals/**.c",
	}
