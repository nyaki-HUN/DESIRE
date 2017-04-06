project "UnitTest"
	AddCommonProjectConfig();
	uuid "157414AA-AF20-4A4C-BA53-CD8CC77C4775"
	kind "ConsoleApp"
	targetdir("../")

	rtti "On"
	flags { "WinMain" }

	pchheader "stdafx.h"
	pchsource "../src/stdafx.cpp"

	includedirs
	{
		"../3rdparty",
		"../../DESIRE-Engine/src",
		"../../DESIRE-Engine/3rdparty",
	}

	filter "files:../src/UnitTest.cpp"
		flags { "NoPCH" }
	filter {}

	links
	{
		"DESIRE",
	}
