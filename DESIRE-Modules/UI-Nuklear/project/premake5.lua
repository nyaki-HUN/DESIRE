project "UI-Nuklear"
	AddModuleConfig()
	uuid "0D3BCC89-786D-4025-AE4B-604C0B57413B"

	pchheader "stdafx_Nuklear.h"
	pchsource "../src/stdafx_Nuklear.cpp"

	defines
	{
		"NK_INCLUDE_FIXED_TYPES",
		"NK_INCLUDE_VERTEX_BUFFER_OUTPUT",
		"NK_INCLUDE_FONT_BAKING",
		"NK_INCLUDE_DEFAULT_FONT",
		"NK_KEYSTATE_BASED_INPUT",
	}

	includedirs
	{
		"../Externals/Nuklear/include",
	}

	files
	{
		"../Externals/Nuklear/**.h",
	}

	filter "files:../src/NuklearImpl.cpp"
		flags { "NoPCH" }
	filter {}
