configurations { "Debug", "Release" }
architecture "x86_64"
language "C++"
characterset "MBCS"
floatingpoint "Fast"
rtti "Off"
editandcontinue "Off"
warnings "Extra"

buildoptions { "/Gw", "/Zc:rvalueCast" }
flags { "StaticRuntime", "MultiProcessorCompile", "C++11" }

if _ACTION == "vs2017" then
	systemversion "10.0.10240.0"
	defines { "WIN32", "_HAS_EXCEPTIONS=0" }
end

function AddCommonLibConfig()
	kind "StaticLib"
	location(_ACTION)
	objdir(_ACTION .. "/obj/%{cfg.buildcfg}/")
	targetdir("../lib/" .. _ACTION)

	defines { "_LIB" }

	includedirs
	{
		"../include",
		"../src",
	}

	files
	{
		"../include/**.h",
		"../include/**.hpp",
		"../src/**.h",
		"../src/**.hpp",
		"../src/**.c",
		"../src/**.cpp",
	}
end

configuration "Debug"
	targetsuffix "_d"
	defines { "DEBUG", "_DEBUG" }
	buildoptions { "/Zc:inline" }
	exceptionhandling "SEH"
	symbols "On"

configuration "Release"
	defines { "NDEBUG" }
	buildoptions { "/Zc:strictStrings" }
	flags { "LinkTimeOptimization", "NoBufferSecurityCheck" }
	exceptionhandling "Off"
	optimize "On"

solution "DESIRE"
	startproject "SandBox"

	include "DESIRE-Engine/project/premake5.lua"
	include "SandBox/project/premake5.lua"
