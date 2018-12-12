cbuffer CB0
{
	matrix WVPMatrix;//64 bytes
};

TextureCube			texture0;
SamplerState		sampler0;

struct VOut
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
	float3 texcoord : TEXCOORD;
	float3 normal : NORMAL;
};

VOut SkyBoxVS(float4 position : POSITION, float4 color : COLOR, float3 texcoord : TEXCOORD, float3 normal : NORMAL)
{
	VOut output;

	output.position = mul(WVPMatrix, position);

	output.color = color;

	output.texcoord = position.xyz;
	output.normal = normalize(normal);

	return output;
}

float4 SkyBoxPS(in VOut input) :SV_TARGET
{
	return input.color * texture0.Sample(sampler0, input.texcoord);
}