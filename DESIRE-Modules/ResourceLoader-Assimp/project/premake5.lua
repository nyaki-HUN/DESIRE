solution "ResourceLoader-Assimp"
	location(_ACTION)

	project "ResourceLoader-Assimp"
		AddModuleConfig()
		uuid "AE7FBF83-FF16-4E32-9844-6CFB79B22929"

		rtti "On"
		defines { "OPENDDL_STATIC_LIBARY", "ASSIMP_BUILD_NO_OWN_ZLIB", "_SCL_SECURE_NO_WARNINGS" }

		includedirs
		{
			"../Externals/Assimp/include",
			"../Externals/Assimp/contrib",
			"../Externals/Assimp/contrib/openddlparser/include",
			"../../Compression-zlib-ng/Externals/zlib-ng/include",
		}

		files
		{
			"../Externals/**.h",
			"../Externals/**.hpp",
			"../Externals/**.cpp",
		}
