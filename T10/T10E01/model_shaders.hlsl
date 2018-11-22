cbuffer CB0
{
	matrix WVPMatrix;//64 bytes
	float4 directional_light_vector;
	float4 directional_light_colour;
	float4 ambient_light_colour;
	float4 point_light_position;
	float4 point_light_colour;
};//total = 64 bytes

Texture2D			texture0;
SamplerState		sampler0;

struct VOut
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
	float2 texcoord : TEXCOORD;
};

VOut ModelVS(float4 position : POSITION, float2 texcoord : TEXCOORD, float3 normal : NORMAL)
{
	VOut output;

	float4 default_color = { 1.0,1.0,1.0,1.0 };
	output.position = mul(WVPMatrix, position);

	float diffuse_amount = dot(directional_light_vector, normal);

	diffuse_amount = saturate(diffuse_amount);

	float4 lightvector = point_light_position - position;		// Get vector from vertex light
	float point_amount = dot(normalize(lightvector), normal);	//Normalise light vector then get
																//dot product with vertex normal
	point_amount = saturate(point_amount);						//Ensure values between 0 and 1

	output.color = ambient_light_colour + (directional_light_colour * diffuse_amount) + (point_light_colour * point_amount);

	output.texcoord = texcoord;

	return output;
}

float4 ModelPS(float4 position : SV_POSITION, float4 color : COLOR, float2 texcoord : TEXCOORD) :SV_TARGET
{
	return color * texture0.Sample(sampler0, texcoord);
}