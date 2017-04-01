configurations { "Debug", "Release" }
architecture "x86_64"
language "C++"
characterset "MBCS"
floatingpoint "Fast"
rtti "Off"
editandcontinue "Off"

systemversion "10.0.10240.0"

defines { "WIN32", "_HAS_EXCEPTIONS=0" }
buildoptions { "/Gw", "/Zc:rvalueCast" }
flags { "StaticRuntime", "MultiProcessorCompile", "C++11" }

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
