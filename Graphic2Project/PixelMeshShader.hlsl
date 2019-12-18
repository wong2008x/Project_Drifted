#pragma pack_matrix(row_major)

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
		float3 Tan  : TANGENT;
		float3 biTan  : BITANGENT;
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
static const float4 amLightClr = { 0.1f,0.1f,0.1f,1.0f };

float4 main(OutputVertex input) : SV_TARGET
{
	float4 SpecClr = {1,1,1,32};
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


	textureColor = txDiffuse.Sample(samLinear, input.Tex);
	if (hasMultiTex)
	{
		texture2Color = txSpecular.Sample(samLinear, input.Tex);
		textureColor = textureColor * texture2Color * 2.0;
	}

	if (hasNormal == true)
	{
		float4 normalMap = txNorm.Sample(samLinear, input.Tex);
		normalMap = (2.0f * normalMap) - 1.0f;
		float3 tangent = normalize(input.Tan-dot(input.Tan,input.Norm)*input.Norm);
		float3 biTangent = cross(input.Norm,input.Tan);
		//float3 tangent = normalize(input.Tan);
		//float3 biTangent = normalize(input.biTan);
		float3x3 texSpace = float3x3(tangent, biTangent, input.Norm);
		input.Norm = (mul((float3)normalMap, texSpace));

	}

		float4 amClr = textureColor * amLightClr;
		//Directional Light
		lightIntensity = saturate(dot(input.Norm, -dLightDir.xyz));
		dirColor = textureColor * dLightClr * lightIntensity;
		float3 toCam = normalize(camPos.xyz - input.PosW);

		if (lightIntensity > 0.0f)
		{
			
			float3 reflection = reflect(dLightDir.xyz, input.Norm);
			float specDot = saturate(dot(reflection, toCam));
			specDot = pow(specDot, SpecClr.w);
			dirColor = dirColor + specDot * SpecClr;
		}

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
		float sAngularATT = saturate(-dot(normalize(sLightDir.xyz), spotLDir));
		float spotFactor = saturate((sAngularATT - outerAngle) / (innerAngle - outerAngle));
		spotColor = textureColor *sLightClr * spotDot * spotFactor;
	
		if (LightingMode == 5)
			return textureColor;

	return saturate(dirColor+ pointColor +spotColor+ amClr);
	
}