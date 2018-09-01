project "Compression-zlib-ng"
	AddModuleConfig()
	uuid "07C8AFBA-5E54-4A5B-BD68-C5C8ED113D57"

	defines
	{
		"Z_SOLO",
		"ZLIB_COMPAT",
		"X86_64",
		"X86_NOCHECK_SSE2",
		"UNALIGNED_OK",
		"UNROLL_LESS",
		"X86_SSE2_FILL_WINDOW",
		"X86_CPUID",
	}

	includedirs
	{
		"../Externals/zlib-ng",
	}

	files
	{
		"../Externals/zlib-ng/*.h",
		"../Externals/zlib-ng/*.c",
		"../Externals/zlib-ng/arch/x86/*.h",
		"../Externals/zlib-ng/arch/x86/*.c",
	}

	removefiles
	{
		"../Externals/zlib-ng/gz*.h",
		"../Externals/zlib-ng/gz*.c",
	}
