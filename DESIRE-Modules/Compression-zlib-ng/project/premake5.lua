solution "Compression-zlib-ng"
	location(_ACTION)

	project "Compression-zlib-ng"
		AddModuleConfig()
		uuid "07C8AFBA-5E54-4A5B-BD68-C5C8ED113D57"

		defines { "Z_SOLO" }

		includedirs
		{
			"../Externals/zlib-ng/include",
		}

		files
		{
			"../Externals/**.h",
			"../Externals/**.c",
		}
