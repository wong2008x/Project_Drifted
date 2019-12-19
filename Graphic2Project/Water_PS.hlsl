#pragma pack_matrix(row_major)

Texture2D txDiffuse : register(t0);
Texture2D txSpecular : register(t1);
Texture2D txNorm : register(t2);
SamplerState samLinear : register(s0);

struct inputVertex
{
	float4 Pos : SV_POSITION;
	float3 Tex : TEXCOORD;
	float3 Norm : NORMAL;
	float3 PosW : WORLDPOS;
	float Timer : TIME;
};

cbuffer LightingConstant : register(b0)
{
	float4 dLightClr;
	float4 dLightDir; //16
	float4 pLightClr;
	float4 pLightPos; //16
	float4 sLightClr;
	float4 sLightDir; //16
	float4 sLightPos; //16
	float innerAngle; //4
	float outerAngle; //4
	float pLightRadius; //4
	int LightingMode; //4
}
cbuffer camConstant : register(b1)
{
	float4 camPos;
	bool hasNormal;
	bool hasMultiTex;
	bool hasShadowMap;
}

static const float4 amLightClr = { 0.8f, 0.8f, 0.8f, 1.0f };

float4 main(inputVertex input) : SV_TARGET
{
	float4 textureColor = {0,0,0,0 };
	
	//Directional Light
	float lightIntensity;
	float4 dirColor = { 0, 0, 0, 0 };
		// SHOULD NORMALIZE INPUT.NORMAL HERE
	input.Norm = normalize(input.Norm);
	input.Tex.xy = float2(float(sin(input.Tex.y * input.Timer) * .01f + input.Tex.x), input.Tex.y);
	textureColor = txDiffuse.Sample(samLinear, input.Tex.xy);
	
	//Directional Light
	lightIntensity = saturate(dot(input.Norm, -dLightDir.xyz));
	dirColor = textureColor * dLightClr * lightIntensity;
	
	float4 amClr = textureColor * amLightClr;
	
	return dirColor+amClr;
}