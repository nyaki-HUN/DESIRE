project "UI-imgui"
	AddModuleConfig()
	uuid "9791305E-BF1B-486C-8405-CFF7B54C98FE"

	pchheader "stdafx_imgui.h"
	pchsource "../src/stdafx_imgui.cpp"

	defines
	{
		"IMGUI_USER_CONFIG=\"custom_imconfig.h\"",
	}

	includedirs
	{
		"../Externals/imgui",
	}

	files
	{
		"../Externals/imgui/**.h",
		"../Externals/imgui/**.cpp",
	}

	filter "action:vs*"
		files
		{
			"../Externals/imgui/**.natvis",
		}
