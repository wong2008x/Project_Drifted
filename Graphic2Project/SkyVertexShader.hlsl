struct OutputVertex
{
	float4 Pos : SV_POSITION;
	float3 Tex : TEXCOORD;
	float3 Norm : NORMAL;
};

cbuffer ConstantBuffer	 : register(b0)
{
	matrix World;
	matrix View;
	matrix Projection;

}

struct skyVertexInput
{
	float3 Pos : POSITION;
	float3 Tex:TEXCOORD;
	float3 Norm:NORMAL;
};


OutputVertex main(skyVertexInput input)
{
	OutputVertex output = (OutputVertex)0;
	output.Pos = float4(input.Pos, 1);
	output.Pos = mul(World, output.Pos); //because it is column major
	output.Pos = mul(View, output.Pos);
	output.Pos = mul(Projection, output.Pos);
	output.Tex = input.Pos.xyz;

	return output;
}