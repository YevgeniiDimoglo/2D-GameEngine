#include "flameFont.hlsli"

Texture2D color_map : register(t0);
SamplerState color_sampler_state : register(s0);
Texture2D mask_texture : register(t1);
Texture2D mask_texture2 : register(t2);
Texture2D mask_texture3 : register(t3);
Texture2D mask_texture4 : register(t4);

float4 main(VS_OUT pin) : SV_TARGET
{
	float4 color = color_map.Sample(color_sampler_state, pin.texcoord) * pin.color;

	float mask_value = mask_texture.Sample(color_sampler_state, pin.texcoord + scroll_directionY * parameters.x);
	float mask_value2 = mask_texture2.Sample(color_sampler_state, pin.texcoord + scroll_directionY * parameters.x);
	float mask_value3 = mask_texture3.Sample(color_sampler_state, pin.texcoord);
	float mask_value4 = mask_texture4.Sample(color_sampler_state, pin.texcoord);

	float mask_result = mask_value * mask_value2 * mask_value3 + mask_value4;

	float yellow = step(0.2, mask_result);
	float orange = step(0.1, mask_result);
	float red = step(0.05, mask_result);

	// red flame

	
	//color.r *= red + yellow + orange;
	//color.g *= yellow + orange * 0.6;
	//color.b *= 0;


	//if (color.r == 0)
	//{
	//	color.a = 0;
	//}
	


	// blue flame

	color.r *= yellow + orange * 1;
	color.g *= yellow;
	color.b *= yellow + orange * 1 + red * 1;


	if (color.b == 0)
	{	
		color.a = 0;
	}

	return color;
}