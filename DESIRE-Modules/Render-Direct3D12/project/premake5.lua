project "Render-Direct3D12"
	AddModuleConfig()
	warnings "Extra"
	uuid "9DE832C6-678D-472D-926A-31560F89AE85"

	pchheader "stdafx_Direct3D12.h"
	pchsource "../src/stdafx_Direct3D12.cpp"

	includedirs
	{
		"../Externals/D3DX12",
	}
