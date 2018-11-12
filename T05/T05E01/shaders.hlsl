cbuffer CBuffer0
{
	matrix WVPMatrix;
	float red_fraction; // 4 bytes
	float scale;
	float2 packing;		
}

struct VOut
{
	float4 position : SV_POSITION;
	float4 color	: COLOR;//Note the spelling of this and all instance below
};

VOut VShader(float4 position : POSITION, float4 color : COLOR, matrix WVPMatrix : WVPMATRIX)
{
	VOut output;
	//color.r *= red_fraction;
	//color.g *= green_fraction;
	output.position = mul(WVPMatrix, position);
	//output.position.xy *= scale;
	output.color = color;

	return output;
}

float4 PShader(float4 position : SV_POSITION, float4 color : COLOR) : SV_TARGET
{
	return color;
}