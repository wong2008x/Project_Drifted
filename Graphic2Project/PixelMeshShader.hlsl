
Texture2D txDiffuse : register(t0);
SamplerState samLinear : register(s0);



float4 main(float4 posH:SV_POSITION, float4 mColor:OCOLOR, float3 Tex : TEXCOORD) : SV_TARGET
{
	return txDiffuse.Sample(samLinear,Tex);
}