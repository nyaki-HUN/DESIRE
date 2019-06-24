project "ResourceLoader-Assimp"
	AddModuleConfig()
	uuid "19E05838-5A9C-421F-9102-F3F5A1ED488A"

	removeflags { "FatalCompileWarnings" }
	rtti "On"

	defines
	{
		"OPENDDL_STATIC_LIBARY",
		"ASSIMP_BUILD_NO_OWN_ZLIB",
		"_SCL_SECURE_NO_WARNINGS",
	}

	includedirs
	{
		"../Externals/Assimp/include",
		"../Externals/Assimp/contrib",
		"../Externals/Assimp/contrib/openddlparser/include",
		"../../Compression-zlib-ng/Externals/zlib-ng",
		"../../Compression-zlib-ng/include",
	}

	files
	{
		"../Externals/**.h",
		"../Externals/**.hpp",
		"../Externals/**.cpp",
	}
