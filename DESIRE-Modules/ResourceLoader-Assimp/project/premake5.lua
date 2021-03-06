project "ResourceLoader-Assimp"
	AddModuleConfig()
	uuid "19E05838-5A9C-421F-9102-F3F5A1ED488A"

	pchheader "stdafx_Assimp.h"
	pchsource "../src/stdafx_Assimp.cpp"

	removeflags { "FatalCompileWarnings" }
	rtti "On"

	defines
	{
		"_SCL_SECURE_NO_WARNINGS",
		"ASSIMP_BUILD_NO_OWN_ZLIB",
		"ASSIMP_BUILD_NO_EXPORT",
		"ASSIMP_BUILD_NO_C4D_IMPORTER",
		"ASSIMP_BUILD_NO_IFC_IMPORTER",
		"OPENDDL_STATIC_LIBARY",
		"RAPIDJSON_HAS_STDSTRING",
	}

	includedirs
	{
		"../Externals/Assimp/",
		"../Externals/Assimp/code",
		"../Externals/Assimp/contrib",
		"../Externals/Assimp/contrib/openddlparser/include",
		"../Externals/Assimp/contrib/pugixml/src",
		"../Externals/Assimp/contrib/unzip",
		"../Externals/Assimp/include",
		"../../Compression-zlib/Externals/zlib",
		"../../../DESIRE-Engine/3rdparty/rapidjson/include",
	}

	files
	{
		"../Externals/**.h",
		"../Externals/**.hpp",
		"../Externals/**.inl",
		"../Externals/**.c",
		"../Externals/**.cpp",
	}

	filter "action:vs*"
		buildoptions { "/bigobj" }
