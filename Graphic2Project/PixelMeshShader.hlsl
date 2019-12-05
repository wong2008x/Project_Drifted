
Texture2D txDiffuse : register(t0);
SamplerState samLinear : register(s0);



struct OutputVertex
{
	
		float4 Pos : SV_POSITION;
		float3 Tex : TEXCOORD;
		float3 Norm : NORMAL;
		float4 PosW:WORLDPOS;
	
};
float4 main(OutputVertex input) : SV_TARGET
{
	float4 diffuseColor = { 0.7f, 0.7f, 0.0f, 1.0f};
	float3 lightDirection = { 0,0,1.0f };
	float4 textureColor;

	float4 pointLColor = { 0.0f,1.0f,1.0f,1.0f };
	float4 pointLPos = { 0.0f,0.5f,0.0f,1.0f };
	float PointLightRadius = 0.05f;
	float4 pLightDir=pointLPos-input.PosW;
	float pDistance = length(pLightDir);
	pLightDir = normalize(pLightDir);

	float lightIntensity;
	float4 color = {0,0,0,0};

	textureColor = txDiffuse.Sample(samLinear, input.Tex);
	lightIntensity = -dot(lightDirection, input.Norm);
	color = saturate(diffuseColor * lightIntensity);

	float fAngularATT = dot(input.Norm, pLightDir);
	float fRangeATT = 1.0 - saturate(pDistance / PointLightRadius);
	color = color * textureColor;
	color += color * pointLColor * fAngularATT * fRangeATT;

	return color;
	
}