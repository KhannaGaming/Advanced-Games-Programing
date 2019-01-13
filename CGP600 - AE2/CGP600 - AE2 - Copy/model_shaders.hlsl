cbuffer CB0
{
	matrix WVPMatrix;//64 bytes
	matrix World;
	float4 directional_light_vector;
	float4 directional_light_colour;
	float4 ambient_light_colour;
	//float4 point_light_position;
	//float4 point_light_colour;
};//total = 64 bytes

Texture2D			texture0;
SamplerState		sampler0;

struct VOut
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
	float2 texcoord : TEXCOORD;
	float3 normal : NORMAL;
};

VOut ModelVS(float4 position : POSITION, float2 texcoord : TEXCOORD, float3 normal : NORMAL)
{
	VOut output;

	float4 default_color = { 1.0,1.0,1.0,1.0 };
	output.position = mul(WVPMatrix, position);

	//float diffuse_amount = dot(directional_light_vector.xyz, normal);

	//diffuse_amount = saturate(diffuse_amount);

	//float4 lightvector = point_light_position - position;		// Get vector from vertex light
	//float point_amount = dot(normalize(lightvector), normal);	//Normalise light vector then get
																//dot product with vertex normal
	//point_amount = saturate(point_amount);						//Ensure values between 0 and 1
	//output.normal = normal;
	output.normal = mul(normal,(float3x3)World);
	output.normal = normalize(normal);

	output.color = default_color;//ambient_light_colour + (directional_light_colour * diffuse_amount);// +(point_light_colour * point_amount);

	output.texcoord = texcoord;

	return output;
}

float4 ModelPS(in VOut input) :SV_TARGET
{
	
	float diffuse_amount = dot(directional_light_vector.xyz, input.normal);

	diffuse_amount = saturate(diffuse_amount);

	//float4 lightvector = point_light_position - position;		// Get vector from vertex light
	//float point_amount = dot(normalize(lightvector), normal);	//Normalise light vector then get
																//dot product with vertex normal
	//point_amount = saturate(point_amount);						//Ensure values between 0 and 1

	input.color = ambient_light_colour + (directional_light_colour * diffuse_amount);// +(point_light_colour * point_amount);


	return input.color * texture0.Sample(sampler0, input.texcoord);
}