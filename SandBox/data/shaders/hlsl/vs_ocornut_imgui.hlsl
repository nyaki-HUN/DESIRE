cbuffer CB : register(b0)
{
	float2 resolution;
};

struct VSInput
{
	float2 pos : POSITION;
	float2 uv : TEXCOORD0;
	float4 color : COLOR0;
};

struct v2f
{
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD0;
	float4 color : TEXCOORD1;
};

v2f main(VSInput In)
{
	v2f Out;
	float2 pos = 2.0 * In.pos / resolution;
	Out.pos = float4(pos.x - 1.0, 1.0 - pos.y, 0.0, 1.0);
	Out.uv = In.uv;
	Out.color = In.color;
	return Out;
}
