cbuffer CB0
{
	matrix WVPMatrix;//64 bytes	
	float4 colour;
};//total = 64 bytes


struct VOut
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
	float2 texcoord : TEXCOORD;
};

VOut ParticleVS(float4 position : POSITION)
{
	VOut output;

	output.position = mul(WVPMatrix, position);

	

	output.color = colour;

	output.texcoord = position.xyz;

	return output;
}

float4 ParticlePS(in VOut input) :SV_TARGET
{
	float distsq = input.texcoord.x*input.texcoord.x + input.texcoord.y*input.texcoord.y;
	clip(1.f - distsq);

	return input.color;// *texture0.Sample(sampler0, input.texcoord);
}