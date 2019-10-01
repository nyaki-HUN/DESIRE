project "Compression-zlib"
	AddModuleConfig()
	uuid "07C8AFBA-5E54-4A5B-BD68-C5C8ED113D57"

	removeflags { "FatalCompileWarnings" }

	defines
	{
		"Z_SOLO",
		"ZLIB_CONST",
	}

	includedirs
	{
		"../Externals/zlib",
	}

	files
	{
		"../Externals/zlib/*.h",
		"../Externals/zlib/*.c",
	}

	removefiles
	{
		"../Externals/zlib/gz*.h",
		"../Externals/zlib/gz*.c",
	}
