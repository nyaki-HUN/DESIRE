project "Network-curl"
	AddModuleConfig()
	uuid "B908992D-EAFC-4E3E-9D31-651D2A8EBA3D"

	includedirs
	{
		"../Externals/curl",
	}

	files
	{
		"../Externals/curl/**.h",
		"../Externals/curl/**.c",
	}
