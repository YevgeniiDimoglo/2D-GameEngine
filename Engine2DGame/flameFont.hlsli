struct VS_OUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
	float2 texcoord : TEXCOORD;
};

cbuffer SCROLL_CONSTANT_BUFFER : register(b1)
{
	float2 scroll_directionX;
	float2 scroll_directionY;
}

cbuffer TIMER_CONSTANT_BUFFER : register(b2)
{
	float4 parameters;
}