struct VS_OUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
	float2 texcoord : TEXCOORD;
};

cbuffer DISSOLVE_CONSTANT_BUFFER : register(b1)
{
	float4 parameters;
}