cbuffer CBuffer0
{
	float scale;
	float red_fraction; // 4 bytes
	float green_fraction; // 4 bytes
	float3 packing;		//3x4 bytes = 12 bytes
}

struct VOut
{
	float4 position : SV_POSITION;
	float4 color	: COLOR;//Note the spelling of this and all instance below
};

VOut VShader(float4 position : POSITION, float4 color : COLOR)
{
	VOut output;
	/*color.r *= red_fraction;
	color.g *= green_fraction;*/
	output.position = position;
	output.position.xy *= scale;
	output.color = color;

	return output;
}

float4 PShader(float4 position : SV_POSITION, float4 color : COLOR) : SV_TARGET
{
	return color;
}