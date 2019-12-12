TextureCube txCube : register(t0);
SamplerState samLinear : register(s0);

struct OutputVertex
{
	float4 Pos : SV_POSITION;
	float3 Tex : TEXCOORD;
	float3 Norm : NORMAL;
}


float4 main(float4 Pos : SV_POSITION, float3 Tex : TEXCOORD) : SV_TARGET
{
	return Pos;
}