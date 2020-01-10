project "Script-Lua"
	AddModuleConfig()
	uuid "9E4F710B-37CB-4F12-ABB8-6A09B0538C17"

	removeflags { "FatalCompileWarnings" }

	includedirs
	{
		"../Externals/lua/src",
		"../Externals/sol/include"
	}

	files
	{
		"../Externals/**.h",
		"../Externals/**.hpp",
		"../Externals/**.c",
	}
