cbuffer CB0
{
	matrix WVPMatrix;//64 bytes
	matrix World;    
};//total = 64 bytes

Texture2D texture0;
SamplerState sampler0;

struct VOut
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
	float2 texcoord : TEXCOORD;
    float3 normal : NORMAL;
};



VOut ParticleVS(float4 position : POSITION, float2 texcoord : TEXCOORD, float3 normal : NORMAL)
{
	VOut output;

	float4 default_color = { 1.0,1.0,1.0,1.0 };

	output.position = mul(WVPMatrix, position);	

    output.normal = mul(normal, (float3x3) World);
    output.normal = normalize(normal);

    output.color = default_color;

	output.texcoord = texcoord;

	return output;
}

float4 ParticlePS(in VOut input) :SV_TARGET
{
//	float distsq = input.texcoord.x*input.texcoord.x + input.texcoord.y*input.texcoord.y;
	//clip(1.f - distsq);

    return input.color * texture0.Sample(sampler0, input.texcoord);
}