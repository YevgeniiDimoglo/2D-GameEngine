#pragma once

#include <windows.h>
#include <tchar.h>
#include <sstream>

#include <d3d11.h>
#include <wrl.h>
#include <WICTextureLoader.h>

#include "misc.h"
#include "high_resolution_timer.h"
#include "sprite.h"
#include "shader.h"
#include "texture.h"

class DissolveShader
{
public:
	DissolveShader();
	~DissolveShader();

	void init(Microsoft::WRL::ComPtr<ID3D11Device> device);
	void render(Microsoft::WRL::ComPtr<ID3D11Device> device, Microsoft::WRL::ComPtr<ID3D11DeviceContext> immediate_context, float elapsed_time);

private:

	struct timer_constants
	{
		DirectX::XMFLOAT4 parameters;
	};

	Microsoft::WRL::ComPtr<ID3D11Buffer> timer_constant_buffer;

	D3D11_TEXTURE2D_DESC mask_texture2dDesc;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mask_texture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mask_texture_noise1;

	Microsoft::WRL::ComPtr<ID3D11VertexShader> sprite_vertex_shader_dissolve;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> sprite_input_layout_dissolve;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> sprite_pixel_shader_dissolve;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler_state_dissolve;

	std::unique_ptr<sprite> background;

};