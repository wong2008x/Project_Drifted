#pragma pack_matrix(row_major)

struct InputStruct
{
	float3 Pos : POSITION;
	float3 Tex : TEXCOORD;
	float3 Norm : NORMAL;
	float4 Tan : TANGENT;
};

struct OutputVertex
{
	float4 Pos : SV_POSITION;
	float3 Tex : TEXCOORD;
	float3 Norm : NORMAL;
	float3 PosW : WORLDPOS;
	float3 Tan : TANGENT;
	float3 biTan : BITANGENT;
};

cbuffer ConstantBuffer : register(b0)
{
	matrix World;
	matrix View;
	matrix Projection;

}

cbuffer instanceBuffer : register(b1)
{
    float3 instancePos;
}

OutputVertex main(InputStruct input, float3 instancePos:INSTANCEPOS)
{
	OutputVertex output = (OutputVertex) 0;
	input.Pos += instancePos;
	output.Pos = float4(input.Pos, 1);
	output.Pos = mul(output.Pos, World); //because it is column major
	//for lighting
	output.PosW = output.Pos.xyz;
	output.Norm = mul(float4(input.Norm, 0), World).xyz;
	output.Pos = mul(output.Pos, View);
	output.Pos = mul(output.Pos, Projection);
	
	output.Tex = input.Tex;

	return output;
}