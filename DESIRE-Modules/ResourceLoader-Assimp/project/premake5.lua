project "ResourceLoader-Assimp"
	AddModuleConfig()
	uuid "19E05838-5A9C-421F-9102-F3F5A1ED488A"

	removeflags { "FatalCompileWarnings" }
	rtti "On"

	defines
	{
		"OPENDDL_STATIC_LIBARY",
		"_SCL_SECURE_NO_WARNINGS",
		"ASSIMP_BUILD_NO_OWN_ZLIB",
		"ASSIMP_BUILD_NO_EXPORT",
		"ASSIMP_BUILD_NO_C4D_IMPORTER",
		"ASSIMP_BUILD_NO_IFC_IMPORTER",
		"ASSIMP_BUILD_NO_STEP_IMPORTER",
	}

	includedirs
	{
		"../Externals/Assimp/",
		"../Externals/Assimp/code",
		"../Externals/Assimp/include",
		"../Externals/Assimp/contrib",
		"../Externals/Assimp/contrib/irrXML",
		"../Externals/Assimp/contrib/openddlparser/include",
		"../Externals/Assimp/contrib/unzip",
		"../../Compression-zlib/Externals/zlib",
		"../../../DESIRE-Engine/3rdparty/rapidjson/include",
	}

	files
	{
		"../Externals/**.h",
		"../Externals/**.hpp",
		"../Externals/**.c",
		"../Externals/**.cpp",
	}

	filter "action:vs*"
		buildoptions { "/bigobj" }
