cbuffer CB0
{
	matrix WVPMatrix;//64 bytes
	float4 colour;// 16 bytes
};

Texture2D			texture0;
SamplerState		sampler0;

struct VOut
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
	float2 texcoord : TEXCOORD;
};

VOut SkyBoxVS(float4 position : POSITION, float2 texcoord : TEXCOORD, float3 normal : NORMAL)
{
	VOut output;

	output.position = mul(WVPMatrix, position);

	output.color = colour;

	output.texcoord = texcoord;

	return output;
}

float4 SkyBoxPS(float4 position : SV_POSITION, float4 color : COLOR, float2 texcoord : TEXCOORD) :SV_TARGET
{
	return color * texture0.Sample(sampler0, texcoord);
}