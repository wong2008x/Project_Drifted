struct OutputVertex
{
	float4 Pos : SV_POSITION;
	float4 Color : OCOLOR;
};

float4 main(OutputVertex inputPixel) : SV_TARGET
{
	return inputPixel.Color;
}