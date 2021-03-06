configurations { "Debug", "Release" }
architecture "x86_64"
language "C++"
cppdialect "C++17"
vectorextensions "AVX"
characterset "MBCS"
stringpooling "On"
floatingpoint "Fast"
rtti "Off"
editandcontinue "Off"
staticruntime "On"

buildoptions
{
	"/Gw",
	"/Zc:rvalueCast",
}

flags
{
	"MultiProcessorCompile",
	"FatalCompileWarnings",
}

filter { "system:windows", "action:vs*" }
	systemversion "10.0.19041.0"
	defines
	{
		"WIN32",
		"_CRT_SECURE_NO_DEPRECATE",
	}

filter "Debug"
	targetsuffix "_d"
	defines
	{
		"DEBUG",
		"_DEBUG",
	}
	buildoptions
	{
		"/Zc:inline",
	}
	exceptionhandling "SEH"
	symbols "On"

filter "Release"
	defines
	{
		"NDEBUG",
	}
	buildoptions
	{
		"/Zc:strictStrings",
	}
	flags
	{
		"LinkTimeOptimization",
		"NoBufferSecurityCheck",
	}
	exceptionhandling "Off"
	optimize "On"

filter {}	-- Reset filter


function AddCommonProjectConfig()
	location(_ACTION)
	objdir(_ACTION .. "/obj/%{cfg.buildcfg}/")
	warnings "Extra"

	includedirs
	{
		"../src",
	}

	files
	{
		"../src/**.h",
		"../src/**.hpp",
		"../src/**.c",
		"../src/**.cpp",
		"../src/**.m",
		"../src/**.mm",
	}
end

function AddCommonLibConfig()
	AddCommonProjectConfig()
	warnings "Default"
	kind "StaticLib"
	targetdir("../lib/" .. _ACTION)

	defines
	{
		"_LIB"
	}

	includedirs
	{
		"../include",
	}

	files
	{
		"../include/**.h",
		"../include/**.hpp",
	}
end

function AddModuleConfig()
	AddCommonLibConfig();
	includedirs
	{
		"../../../DESIRE-Engine/src",
	}

	filter { "files:../Externals/**.cpp or ../Externals/**.c" }
		flags { "NoPCH" }
	filter {}	-- Reset filter
end

-- Solution and projects
solution "DESIRE"
	startproject "SandBox"

	include "DESIRE-Engine/project/premake5.lua"
	include "SandBox/project/premake5.lua"
	include "UnitTest/project/premake5.lua"

	-- Compression modules
	include "DESIRE-Modules/Compression-Lizard/project/premake5.lua"
	include "DESIRE-Modules/Compression-LZ4/project/premake5.lua"
	include "DESIRE-Modules/Compression-zlib/project/premake5.lua"
	include "DESIRE-Modules/Compression-zstd/project/premake5.lua"

	-- Network module
	include "DESIRE-Modules/Network-curl/project/premake5.lua"

	-- Physics modules
	include "DESIRE-Modules/Physics-Box2D/project/premake5.lua"
	include "DESIRE-Modules/Physics-Bullet/project/premake5.lua"
	include "DESIRE-Modules/Physics-PhysX/project/premake5.lua"

	-- Render modules
	include "DESIRE-Modules/Render-bgfx/project/premake5.lua"
	include "DESIRE-Modules/Render-Direct3D11/project/premake5.lua"
	include "DESIRE-Modules/Render-Direct3D12/project/premake5.lua"

	-- ResourceLoader modules
	include "DESIRE-Modules/ResourceLoader-Assimp/project/premake5.lua"

	-- Script modules
	include "DESIRE-Modules/Script-AngelScript/project/premake5.lua"
	include "DESIRE-Modules/Script-Lua/project/premake5.lua"
	include "DESIRE-Modules/Script-Squirrel/project/premake5.lua"

	-- UI modules
	include "DESIRE-Modules/UI-imgui/project/premake5.lua"
	include "DESIRE-Modules/UI-Nuklear/project/premake5.lua"
