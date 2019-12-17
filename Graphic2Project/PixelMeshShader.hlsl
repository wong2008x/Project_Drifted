
Texture2D txDiffuse : register(t0);
Texture2D txSpecular: register(t1);
Texture2D txNorm:register(t2);
SamplerState samLinear : register(s0);



struct OutputVertex
{
		float4 Pos : SV_POSITION;
		float3 Tex : TEXCOORD;
		float3 Norm : NORMAL;
		float3 PosW:WORLDPOS;
};

cbuffer LightingConstant :register(b0)
{
	float4 dLightDir;  //16
	float4 pLightPos;//16
	float4 sLightDir;//16
	float4 sLightPos;//16
	float innerAngle;//4
	float outerAngle;//4
	float pLightRadius;//4
	int LightingMode;//4
}
static const float4 diffuseColor = { 1.0f, 1.0f, 0.878f, 1.0f };
static const float4 pointLColor = { 0.9f,0.0f,0.0f,1.0f };
static const float4 spotLColor = { 0.8f,0.8f,0.8f,1.0f };
static const float4 amLightClr = { 0.2f, 0.2f, 0.2f,1.0f };

float4 main(OutputVertex input) : SV_TARGET
{
	float4 textureColor;
	float4 texture2Color;
	
	//Directional Light
	float   lightIntensity;
	float4 dirColor = { 0,0,0,0 };
	//Point Light
	float3 pLightDir;
	float4 pointColor = { 0,0,0,0 };
	float4 spotColor = { 0,0,0,0 };
	// SHOULD NORMALIZE INPUT.NORMAL HERE
	input.Norm = normalize(input.Norm);


	textureColor = txDiffuse.Sample(samLinear, input.Tex.xy);
	//texture2Color = txSpecular.Sample(samLinear, input.Tex.xy);

	//	textureColor = textureColor * texture2Color * 2.0;
	float4 amClr = textureColor * amLightClr;
	//Directional Light
		lightIntensity = saturate(dot(input.Norm, -dLightDir.xyz));
		dirColor = textureColor* saturate(diffuseColor * lightIntensity);
	//Point Light
		pLightDir = pLightPos.xyz - input.PosW;
		float pDistance = length(pLightDir);
		pLightDir /= pDistance;
		float pAngularATT = dot(input.Norm, pLightDir);
		float pRangeATT = 1.0 - saturate(pDistance / pLightRadius);
		pointColor += textureColor * pointLColor * pAngularATT * pRangeATT;
	
	//Spot LIght

		float3 spotLDir =normalize(sLightPos.xyz - input.PosW);
		float spotDot = dot(input.Norm, spotLDir);
		float sAngularATT = saturate(-dot(sLightDir.xyz, spotLDir));
		float spotFactor = saturate((sAngularATT - outerAngle) / (innerAngle - outerAngle));
		spotColor = textureColor *spotLColor * spotDot * spotFactor;
	
		if (LightingMode == 5)
			return textureColor;

	return dirColor+ pointColor +spotColor+ amClr;
	
}