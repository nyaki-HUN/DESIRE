project "Script-AngelScript"
	AddModuleConfig()
	warnings "Extra"
	uuid "AE7FBF83-FF16-4E32-9844-6CFB79B22929"

	pchheader "stdafx_AngelScript.h"
	pchsource "../src/stdafx_AngelScript.cpp"

	exceptionhandling "On"

	includedirs
	{
		"../Externals/AngelScript/include",
	}

	files
	{
		"../Externals/**.h",
		"../Externals/**.cpp",
	}

	filter "action:vs*"
		files
		{
			"../Externals/AngelScript/src/as_callfunc_x64_msvc_asm.asm"
		}

	filter { "action:vs*", "files:../Externals/AngelScript/src/**.asm" }
		buildcommands "ml64.exe /c  /nologo /Fo%{cfg.objdir}/%{file.basename}.obj /W3 /Zi /Ta %{file.relpath}"
		buildoutputs "%{cfg.objdir}/%{file.basename}.obj;%(Outputs)"

	filter "Debug"
		defines
		{
			"AS_DEBUG",
		}
