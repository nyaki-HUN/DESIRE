project "Script-Lua"
	AddModuleConfig()
	uuid "9E4F710B-37CB-4F12-ABB8-6A09B0538C17"

	pchheader "stdafx_Lua.h"
	pchsource "../src/stdafx_Lua.cpp"

	defines
	{
		"SOL_NO_EXCEPTIONS=1",
	}

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

	filter "action:vs*"
		buildoptions { "/bigobj" }
