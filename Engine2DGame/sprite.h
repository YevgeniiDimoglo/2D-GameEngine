#pragma once

#include <d3d11.h>
#include <directxmath.h>

#include <wrl.h>
#include <string>

class sprite
{
private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_view;
	D3D11_TEXTURE2D_DESC texture2d_desc;

	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader = nullptr;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader = nullptr;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout = nullptr;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState = nullptr;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState = nullptr;

public:
	struct vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT4 color;
		DirectX::XMFLOAT2 texcoord;
	};

	sprite(ID3D11Device* device, const wchar_t* filename);
	virtual ~sprite();

	void render(ID3D11DeviceContext* immediate_context, float dx, float dy, float dw, float dh, float r, float g, float b, float a, float angle/*degree*/);
	void render(ID3D11DeviceContext* immediate_context, float dx, float dy, float dw, float dh, float r, float g, float b, float a, float angle/*degree*/, float sx, float sy, float sw, float sh);
	void render(ID3D11DeviceContext* immediate_context, float dx, float dy, float dw, float dh);
	void render(ID3D11DeviceContext* immediate_context, const DirectX::XMFLOAT2& position, const DirectX::XMFLOAT2& scale, const DirectX::XMFLOAT2& texPos, const DirectX::XMFLOAT2& texSize, const DirectX::XMFLOAT2& center, float angle, const DirectX::XMFLOAT4& color) const;
	void textout(ID3D11DeviceContext* immediate_context, std::string s, float x, float y, float w, float h, float r, float g, float b, float a);
	void textout2(ID3D11DeviceContext* immediate_context, std::string s, float x, float y, float w, float h, float r, float g, float b, float a);
	void circle(ID3D11DeviceContext* immediate_context, const DirectX::XMFLOAT2& center, float radius, const DirectX::XMFLOAT2& scale, float angle, const DirectX::XMFLOAT4& color, int n);
};
