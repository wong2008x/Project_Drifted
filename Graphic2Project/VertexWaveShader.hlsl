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
	float3 PosW:WORLDPOS;
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

	float multiplyValue = 1.2f+abs(sin(timer * 5 + output.Pos.y)); //how much we want to multiply our vertex
	output.Pos.x *= multiplyValue ;
	output.Pos.z *= multiplyValue ;
	/*output.Pos.x = output.Pos.x * sin(output.Pos.y*0.1f+timer*5.f);
	output.Pos.z = output.Pos.z * sin(output.Pos.y * 0.1f + timer * 2.f);*/
	output.Pos = mul(World, output.Pos); //because it is column major
	//for lighting
	output.PosW = output.Pos.xyz;
	output.Norm = mul(World, float4(input.Norm, 0)).xyz;

	output.Pos = mul(View, output.Pos);
	output.Pos = mul(Projection, output.Pos);
	output.Tex = input.Tex;

	return output;
}