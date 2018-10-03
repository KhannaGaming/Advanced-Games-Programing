struct VOut
{
	float4 position : SV_POSITION;
	float4 color	: COLOR;//Note the spelling of this and all instance below
};

VOut VShader(float4 position : POSITION, float4 color : COLOR)
{
	VOut output;

	output.position = position;
	output.color.r = color.r + position.x;
	output.color.g = color.g + (position.x*position.y);
	output.color.b = color.b + (position.x*position.y);

	return output;
}

float4 PShader(float4 position : SV_POSITION, float4 color : COLOR) : SV_TARGET
{
	return color;
}