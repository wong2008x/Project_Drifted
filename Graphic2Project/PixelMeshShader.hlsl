
Texture2D txDiffuse : register(t0);
SamplerState samLinear : register(s0);



struct OutputVertex
{
		float4 Pos : SV_POSITION;
		float3 Tex : TEXCOORD;
		float3 Norm : NORMAL;
		float3 PosW:WORLDPOS;
};
float4 main(OutputVertex input) : SV_TARGET
{

	float4 textureColor;

//Directional Light
	float4 diffuseColor = { 0.9f, 0.9f, 0.9f, 1.0f};
	float3 lightDirection = { -0.557f,-0.557f,0.557f };
	float   lightIntensity;
	float4 dirColor = { 0,0,0,0 };

//Point Light
	float4 pointLColor = { 0.9f,0.0f,0.0f,1.0f };
	float3 pointLPos = { -5.0f,0.5f,1.0f };

	float PointLightRadius = 10.0f;
	float4 pointColor = { 0,0,0,0 };


	//Spot Light
	float3 spotPos = { 0.0f,4.0f,0.0f };
	float3 spotLDir = { 0.0f,-1.0f,0.0f };
	float innerAngle = 0.9f;
	float outerAngle = 0.8f;
	float4 spotLColor = { 0.0f,0.0f,0.7f,1.0f };
	float4 spotColor = { 0,0,0,0 };


	// SHOULD NORMALIZE INPUT.NORMAL HERE
	input.Norm = normalize(input.Norm);

	//Apply Texture
	input.Tex.xy= float2(float(sin(input.Tex.x * 500) * .01f + input.Tex.x), input.Tex.y);
	textureColor = txDiffuse.Sample(samLinear, input.Tex.xy);
	
	//Directional Light
	lightIntensity = saturate(dot( input.Norm, -lightDirection));
	dirColor = textureColor* saturate(diffuseColor * lightIntensity);

	//Point Light
	float3 pLightDir = pointLPos - input.PosW;
	float pDistance = length(pLightDir);
	pLightDir /= pDistance;
	float pAngularATT = dot(input.Norm, pLightDir);
	float pRangeATT = 1.0 - saturate(pDistance / PointLightRadius);
	pointColor += textureColor * pointLColor * pAngularATT * pRangeATT;

	//Spot LIght
	float3 sLightDir =normalize(spotPos - input.PosW);

	float spotDot = dot(input.Norm, sLightDir);
	float sAngularATT = saturate(-dot(sLightDir, spotLDir));
	float spotFactor = saturate((sAngularATT - outerAngle) / (innerAngle - outerAngle));
	spotColor = textureColor *spotLColor * spotDot * spotFactor;


	return dirColor+ pointColor +spotColor;
	
}