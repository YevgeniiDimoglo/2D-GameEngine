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

class CloudShader
{
public:
	CloudShader();
	~CloudShader();

	void init(Microsoft::WRL::ComPtr<ID3D11Device> device, std::shared_ptr<sprite>& sprite);
	void render(Microsoft::WRL::ComPtr<ID3D11Device> device, Microsoft::WRL::ComPtr<ID3D11DeviceContext> immediate_context, float elapsed_time);

private:

	struct timer_constants
	{
		DirectX::XMFLOAT4 parameters;
	};

	struct scroll_constants
	{
		DirectX::XMFLOAT2 scroll_directionX;
		DirectX::XMFLOAT2 scroll_directionY;
	};

	struct size_constants
	{
		DirectX::XMFLOAT2 size;
		DirectX::XMFLOAT2 dummy;
	};

	Microsoft::WRL::ComPtr<ID3D11Buffer> timer_constant_buffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> scroll_constant_buffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> size_constant_buffer;

	Microsoft::WRL::ComPtr<ID3D11VertexShader> sprite_vertex_shader_cloud;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> sprite_input_layout_cloud;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> sprite_pixel_shader_cloud;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler_state_cloud;

	std::shared_ptr<sprite> spriteCloud;

};