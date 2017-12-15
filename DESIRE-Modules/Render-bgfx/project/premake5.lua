solution "Render-bgfx"
	location(_ACTION)

	project "Render-bgfx"
		AddModuleConfig()
		uuid "39620121-0C61-49D9-9BF4-B67638336D45"

		includedirs
		{
			"../Externals/bgfx/include",
			"../Externals/bgfx/3rdparty",
			"../Externals/bgfx/3rdparty/dxsdk/include",
			"../Externals/bgfx/3rdparty/khronos",
			"../Externals/bimg/include",
			"../Externals/bx/include",
		}

		files
		{
			"../Externals/**.h",
			"../Externals/**.cpp",
		}

		removefiles
		{
			"../Externals/**/amalgamated.*",
		}

		filter { "system:windows" }
			includedirs
			{
				"../Externals/bx/include/compat/msvc",
			}
		filter {}

		configuration { "Debug" }
			defines
			{
				"BX_CONFIG_DEBUG=1",
			}

		configuration {}	-- Reset configuration
