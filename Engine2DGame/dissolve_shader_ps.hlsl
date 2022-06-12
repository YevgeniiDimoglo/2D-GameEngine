#include "dissolve_shader.hlsli"

Texture2D color_map : register(t0);
SamplerState color_sampler_state : register(s0);
Texture2D mask_texture_noise1 : register(t1);

float4 main(VS_OUT pin) : SV_TARGET
{
	float4 color = color_map.Sample(color_sampler_state, pin.texcoord) * pin.color;
	float mask_value = mask_texture_noise1.Sample(color_sampler_state, pin.texcoord);
	float alpha = step(parameters.x, mask_value);

	if ((parameters.x - mask_value <= 0.1f) && parameters.x - mask_value >= .0f)
	{
		color.g = 0;
		color.b = 0;
	}
	else
	{
		color.a *= alpha;
	}

	return color;
}