solution "UI-imgui"
	location(_ACTION)

	project "UI-imgui"
		AddModuleConfig()
		uuid "9791305E-BF1B-486C-8405-CFF7B54C98FE"

		includedirs
		{
			"../Externals/imgui",
		}

		files
		{
			"../Externals/imgui/**.h",
			"../Externals/imgui/**.cpp",
		}
