SamplerState samplerState : register(s0);
Texture2D tex : register(t0);

struct v2f
{
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD0;
	float4 color : TEXCOORD1;
};

float4 main(v2f In) : SV_TARGET
{
	return tex.Sample(samplerState, In.uv) * In.color;
}
