TextureCube txCube : register(t0);
SamplerState samLinear : register(s0);



float4 main(float4 Pos : SV_POSITION,
	float3 Tex : TEXCOORD) : SV_TARGET
{
	return txCube.Sample(samLinear,normalize(Tex));
}