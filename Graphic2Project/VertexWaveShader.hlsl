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
	float Timer : TIME;
};

cbuffer ConstantBuffer	 : register(b0)
{
	matrix World;
	matrix View;
	matrix Projection;

}
cbuffer ConstantTimer	 : register(b1)
{
	double timer;
}

OutputVertex main(InputStruct input)
{
	OutputVertex output = (OutputVertex)0;
	output.Pos = float4(input.Pos, 1);

	float multiplyValue =0.8f+2.5f*sin(timer * 2.0f + output.Pos.x); //how much we want to multiply our vertex
	output.Pos.y *= multiplyValue ;

	output.Pos = mul(output.Pos, World); //because it is column major
	//for lighting
	output.PosW = output.Pos.xyz;
	output.Norm = mul(float4(input.Norm, 0), World).xyz;
	output.Pos = mul(output.Pos, View);
	output.Pos = mul(output.Pos, Projection);
	output.Timer = float(timer);
	output.Tex = input.Tex;

	return output;
}