project "Script-Lua"
	AddModuleConfig()
	uuid "9E4F710B-37CB-4F12-ABB8-6A09B0538C17"

	removeflags { "FatalCompileWarnings" }

	includedirs
	{
		"../Externals/lua/src",
		"../Externals/LuaBridge/Source"
	}

	files
	{
		"../Externals/lua/src/*.h",
		"../Externals/lua/src/*.hpp",
		"../Externals/lua/src/*.c",
		"../Externals/LuaBridge/Source/**.h",
	}
