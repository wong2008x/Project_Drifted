
Texture2D txDiffuse : register(t0);
SamplerState samLinear : register(s0);

float4 diffuseColor = { 1.0f, 0.0f, 0.0f, 1.0f};
float3 lightDirection= { 0,0,1.0f };

struct OutputVertex
{
	float4 Pos : SV_POSITION;
	float3 Tex : TEXCOORD;
	float3 Norm : NORMAL;
};
float4 main(OutputVertex input) : SV_TARGET
{
	float4 textureColor;
	//float3 lightDir;
	float lightIntensity;
	float4 color;

	textureColor = txDiffuse.Sample(samLinear, input.Tex);
	lightIntensity = -dot(lightDirection, input.Norm);
	color = saturate(diffuseColor * lightIntensity);

	color = color * textureColor;
	return textureColor;
	
}