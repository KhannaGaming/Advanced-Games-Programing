struct VOut
{
	float4 position : SV_POSITION;
	float4 color	: COLOR;//Note the spelling of this and all instance below
};

VOut VShader(float4 position : POSITION, float4 color : COLOR)
{
	VOut output;

	output.position = position;
	output.color = color;

	return output;
}

float4 PShader(float4 position : SV_POSITION, float4 color : COLOR) : SV_TARGET
{
	return color;
}