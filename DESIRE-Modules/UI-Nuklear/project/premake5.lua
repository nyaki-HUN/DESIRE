project "UI-Nuklear"
	AddModuleConfig()
	uuid "0D3BCC89-786D-4025-AE4B-604C0B57413B"

	pchheader "stdafx_Nuklear.h"
	pchsource "../src/stdafx_Nuklear.cpp"

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
