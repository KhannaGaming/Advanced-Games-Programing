cbuffer CB0
{
	matrix WVPMatrix;//64 bytes
	matrix worldViewMatrix;//64 bytes
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

VOut ReflectionVS(float4 position : POSITION, float4 color : COLOR, float3 texcoord : TEXCOORD, float3 normal : NORMAL)
{
	VOut output;

	output.position = mul(WVPMatrix, position);

	output.color = color;

	//position relative to camera
	float3 wvpos = mul(worldViewMatrix, position);

	//surface normal relative to camera
	float3 wvnormal = mul(worldViewMatrix, normal);
	wvnormal = normalize(wvnormal);

	// obtain the reverse eye vector
	float3 eyer = -normalize(wvpos);

	//compute the reflection vector
	output.texcoord = 2.0 * dot(eyer, wvnormal)*wvnormal - eyer;
	output.normal = normalize(normal);

	return output;
}

float4 ReflectionPS(float4 position : SV_POSITION, float4 color : COLOR, float3 texcoord : TEXCOORD, float3 normal : NORMAL) :SV_TARGET
{
	return color * texture0.Sample(sampler0, texcoord);
}