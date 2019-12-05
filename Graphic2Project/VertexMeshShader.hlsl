

//#pragma pack_matrix(row_major)

struct InputStruct
{
	float3 Pos : POSITION;
	float3 Tex : TEXCOORD;
	float3 Norm : NORMAL;
};

struct OutputVertex
{
	float4 Pos : SV_POSITION;
	float3 Tex : TEXCOORD;
	float3 Norm : NORMAL;
	float4 PosW:WORLDPOS;
};

cbuffer ConstantBuffer	 : register(b0)
{
	matrix World;
	matrix View;
	matrix Projection;

}

OutputVertex main(InputStruct input) 
{
	OutputVertex output = (OutputVertex)0;
	output.Pos = float4(input.Pos,1);
	output.Pos = mul(World, output.Pos ); //because it is column major
	output.PosW = output.Pos;
	output.Pos = mul(View, output.Pos );
	output.Pos = mul(Projection, output.Pos );
	output.Tex = input.Tex;
	output.Norm = mul(World, float4(input.Norm, 1)).xyz;
	output.Norm =normalize(output.Norm);
	return output;
}