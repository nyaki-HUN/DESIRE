cbuffer CB : register(b0)
{
	float4x4 matWorldViewProj;
};

struct VSInput
{
	float3 pos : POSITION;
	float2 uv : TEXCOORD0;
	float4 color : COLOR0;
};

struct v2f
{
	float4 pos : SV_Position;
	float2 uv : TEXCOORD0;
	float4 color : TEXCOORD1;
};

v2f main(VSInput In)
{
	v2f Out;
	Out.pos = mul(matWorldViewProj, float4(In.pos, 1.0));
	Out.uv = In.uv;
	Out.color = In.color;
	return Out;
}
