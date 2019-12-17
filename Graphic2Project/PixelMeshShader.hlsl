
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
	float4 dLightClr ;
	float4 dLightDir;  //16
	float4 pLightClr;
	float4 pLightPos;//16
	float4 sLightClr;
	float4 sLightDir;//16
	float4 sLightPos;//16
	float innerAngle;//4
	float outerAngle;//4
	float pLightRadius;//4
	int LightingMode;//4
}
cbuffer camConstant :register(b1)
{
	float4 camPos;
	bool hasNormal;
	bool hasMultiTex;
	bool hasShadowMap;
}
static const float4 amLightClr = { 0.2f,0.2f,0.2f,1.0f };

float4 main(OutputVertex input) : SV_TARGET
{
	float4 SpecClr = {1,1,1,1};
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
	if (hasMultiTex)
	{
		texture2Color = txSpecular.Sample(samLinear, input.Tex.xy);
		textureColor = textureColor * texture2Color * 2.0;
	}
		float4 amClr = textureColor * amLightClr;
		//Directional Light
		float3 toCam = normalize(camPos.xyz - input.PosW);
		float3 reflection = reflect(dLightDir.xyz, input.Norm);
		float specDot = saturate(dot(reflection,toCam));
		specDot = pow(specDot,32);
		lightIntensity = saturate(dot(input.Norm, -dLightDir.xyz));
		dirColor = textureColor* dLightClr * lightIntensity;

	//Point Light
		pLightDir = pLightPos.xyz - input.PosW;
		float pDistance = length(pLightDir);
		pLightDir /= pDistance;
		float pAngularATT = dot(input.Norm, pLightDir);
		float pRangeATT = 1.0 - saturate(pDistance / pLightRadius);
		pointColor += textureColor * pLightClr * pAngularATT * pRangeATT;
	
	//Spot LIght

		float3 spotLDir =normalize(sLightPos.xyz - input.PosW);
		float spotDot = dot(input.Norm, spotLDir);
		float sAngularATT = saturate(-dot(sLightDir.xyz, spotLDir));
		float spotFactor = saturate((sAngularATT - outerAngle) / (innerAngle - outerAngle));
		spotColor = textureColor *sLightClr * spotDot * spotFactor;
	
		if (LightingMode == 5)
			return textureColor;

	return dirColor+ pointColor +spotColor+ amClr;
	
}