cbuffer CB0
{
	matrix WVPMatrix;//64 bytes
	matrix worldViewMatrix;//64 bytes
	float4 directional_light_vector;
	float4 directional_light_colour;
	float4 ambient_light_colour;
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

VOut ReflectionVS(float4 position : POSITION, float3 texcoord : TEXCOORD, float3 normal : NORMAL)
{
	VOut output;

	output.position = mul(WVPMatrix, position);

	float4 default_color = { 1.0,1.0,1.0,1.0 };
	output.color = default_color;

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

float4 ReflectionPS(in VOut input) :SV_TARGET
{

	float diffuse_amount = dot(directional_light_vector.xyz, input.normal);

	diffuse_amount = saturate(diffuse_amount);

	float3 reflection = 2 * diffuse_amount*input.normal - directional_light_vector.xyz;
	input.color = dot((0.5*input.texcoord),(0.5*reflection));;
	input.color += ambient_light_colour + (directional_light_colour * diffuse_amount);

	return input.color * texture0.Sample(sampler0, input.texcoord);
}