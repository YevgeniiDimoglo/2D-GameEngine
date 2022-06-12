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

class FlameFont
{
public:
	FlameFont();
	~FlameFont();

	void init(Microsoft::WRL::ComPtr<ID3D11Device> device);
	void render(Microsoft::WRL::ComPtr<ID3D11Device> device, Microsoft::WRL::ComPtr<ID3D11DeviceContext> immediate_context, float elapsed_time);

private:

	struct scroll_constants
	{
		DirectX::XMFLOAT2 scroll_directionX;
		DirectX::XMFLOAT2 scroll_directionY;
	};

	struct timer_constants
	{
		DirectX::XMFLOAT4 parameters;
	};

	Microsoft::WRL::ComPtr<ID3D11Buffer> scroll_constant_buffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> timer_constant_buffer;

	D3D11_TEXTURE2D_DESC mask_texture2dDesc;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mask_texture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mask_texture_noise1;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mask_texture_noise2;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mask_texture_flame1;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mask_texture_flame2;

	Microsoft::WRL::ComPtr<ID3D11VertexShader> sprite_vertex_shader_flame;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> sprite_input_layout_flame;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> sprite_pixel_shader_flame;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler_state_flame;

	Microsoft::WRL::ComPtr<ID3D11VertexShader> sprite_vertex_shader_flamefront;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> sprite_input_layout_flamefront;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> sprite_pixel_shader_flamefront;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler_state_flamefront;

	std::unique_ptr<sprite> letter_frontA;
	std::unique_ptr<sprite> letter_backA;
	std::unique_ptr<sprite> letter_frontC;
	std::unique_ptr<sprite> letter_backC;
	std::unique_ptr<sprite> letter_frontE;
	std::unique_ptr<sprite> letter_backE;
	std::unique_ptr<sprite> letter_frontS;
	std::unique_ptr<sprite> letter_backS;

};