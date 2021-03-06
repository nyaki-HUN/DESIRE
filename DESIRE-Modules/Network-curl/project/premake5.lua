project "Network-curl"
	AddModuleConfig()
	uuid "B908992D-EAFC-4E3E-9D31-651D2A8EBA3D"

	pchheader "stdafx_curl.h"
	pchsource "../src/stdafx_curl.cpp"

	defines
	{
		"BUILDING_LIBCURL",
		"CURL_STATICLIB",
		"HTTP_ONLY",
	}

	includedirs
	{
		"../Externals/curl/include",
		"../Externals/curl/src",
	}

	files
	{
		"../Externals/curl/**.h",
		"../Externals/curl/**.c",
	}
