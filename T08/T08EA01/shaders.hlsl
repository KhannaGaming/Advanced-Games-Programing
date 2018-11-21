Texture2D texture0;
Texture2D texture1;
SamplerState sampler0;

cbuffer CBuffer0
{
	matrix WVPMatrix;	// 64 bytes
	float red_fraction; // 4 bytes
	float scale;		// 4 bytes
	float2 packing;		// 8 bytes
}//total 80 bytes

struct VOut
{
	float4 position : SV_POSITION;
	float4 color	: COLOR;//Note the spelling of this and all instance below
	float2 texcoord : TEXCOORD;
};

VOut VShader(float4 position : POSITION, float4 color : COLOR, float2 texcoord : TEXCOORD)
{
	VOut output;
	output.position = mul(WVPMatrix, position);
	output.color = color;
	output.texcoord = texcoord;

	return output;
}

float4 PShader(float4 position : SV_POSITION, float4 color : COLOR, float2 texcoord : TEXCOORD) : SV_TARGET
{
	return color * (texture0.Sample(sampler0,texcoord)+ texture1.Sample(sampler0,texcoord));
}