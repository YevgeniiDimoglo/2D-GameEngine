#pragma target 5.0

#include "cloud_shader.hlsli"

//
// https://www.shadertoy.com/view/tlB3zK
//float2 g(float2 n) { return sin(n.x * n.y * float2(12, 17) + float2(1, 2)); }
float2 g( float2 n ) { return sin(n.x*n.y+float2(0,1.571)); } // if you want the gradients to lay on a circle

float noise(float2 p)
{
	const float kF = 2.0;  // make 6 to see worms

	float2 i = floor(p);
	float2 f = frac(p);
	f = f * f * (3.0 - 2.0 * f);
	return lerp(lerp(sin(kF * dot(p, g(i + float2(0, 0)))),
		sin(kF * dot(p, g(i + float2(1, 0)))), f.x),
		lerp(sin(kF * dot(p, g(i + float2(0, 1)))),
			sin(kF * dot(p, g(i + float2(1, 1)))), f.x), f.y);
}

const float2x2 m2 = float2x2(1.6, 1.2, -1.2, 1.6);

float fbm4(float2 p) {
    float amp = 0.5;
    float h = 0.0;
    for (int i = 0; i < 4; i++) {
        float n = noise(p);
        h += amp * n;
        amp *= 0.5;
        p = mul(m2,p);
    }

    return  0.5 + 0.5 * h;
}

float4 main(VS_OUT pin) : SV_TARGET
{
    // Normalized pixel coordinates (from 0 to 1)
    float2 uv = pin.texcoord / float2(0.5,0.5);
    uv -= 0.5;
    uv.x *= size.x / size.y;
    float3 sky = float3(0.5, 0.7, 0.8);
    float3 col = float3(0.0, 0.0, 0.0);

    // speed
    float v = 0.01;

    // layer1
    float3 cloudCol = float3(1.0, 1.0, 1.0);

    float2 scale = uv * 1.0;
    float2 turbulence = 0.008 * float2(noise(float2(uv.x * 10.0, uv.y * 10.0)), noise(float2(uv.x * 10.0, uv.y * 10.0)));
    scale += turbulence;
    float n1 = fbm4(float2(scale.x - 20.0 * sin(parameters.x * v * 2.0), scale.y - 50.0 * sin(parameters.x * v)));
    col = lerp(sky, cloudCol, smoothstep(0.5, 0.8, n1));

    //layer2
    scale = uv * 0.5;
    turbulence = 0.05 * float2(noise(float2(uv.x * 2.0, uv.y * 2.1)), noise(float2(uv.x * 1.5, uv.y * 1.2)));
    scale += turbulence;
    float n2 = fbm4(scale + 20.0 * sin(parameters.x * v));
    col = lerp(col, cloudCol, smoothstep(0.2, 0.9, n2));
    col = min(col, float3(1.0, 1.0, 1.0));


    // Output to screen
    pin.color = float4(col, 1.0);

    return pin.color;
}
