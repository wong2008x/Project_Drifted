

//#pragma pack_matrix(row_major)
struct InputStruct
{
	float4 Pos : POSITION;
	float4 Color : COLOR;
};

struct OutputVertex
{
	float4 Pos : SV_POSITION;
	float4 Color : OCOLOR;
};

cbuffer ConstantBuffer	 : register(b0)
{
	float4x4 World;
	float4x4 View;
	float4x4 Projection;

}

OutputVertex main(InputStruct input) 
{
	OutputVertex output = (OutputVertex)0;
	output.Pos = input.Pos;
	output.Color = input.Color;
	output.Pos = mul(World, output.Pos );
	output.Pos = mul(View, output.Pos );
	output.Pos = mul(Projection, output.Pos );

	return output;
}