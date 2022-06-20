#include "Player.h"

Player::Player()
{

}

void Player::init(Microsoft::WRL::ComPtr<ID3D11Device> device)
{
	
	pp.pos = { 512, 512 };
	pp.angle = 0;

	spritePlayer = std::make_unique<sprite>(device.Get(), L".\\resources\\player\\player.png");
	spriteJet = std::make_unique<sprite>(device.Get(), L".\\resources\\FlameLetters\\ShapeTextureFull.png");
	

	load_texture_from_file(device.Get(), L".\\resources\\FlameLetters\\NoiseTexture1.png", mask_texture_noise1.GetAddressOf(), &mask_texture2dDesc);
	load_texture_from_file(device.Get(), L".\\resources\\FlameLetters\\NoiseTexture2.png", mask_texture_noise2.GetAddressOf(), &mask_texture2dDesc);
	load_texture_from_file(device.Get(), L".\\resources\\FlameLetters\\FlameTexture1.png", mask_texture_flame1.GetAddressOf(), &mask_texture2dDesc);
	load_texture_from_file(device.Get(), L".\\resources\\FlameLetters\\FlameTexture2.png", mask_texture_flame2.GetAddressOf(), &mask_texture2dDesc);

	HRESULT hr{ S_OK };

	D3D11_BUFFER_DESC buffer_desc{};
	buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	buffer_desc.CPUAccessFlags = 0;
	buffer_desc.MiscFlags = 0;
	buffer_desc.StructureByteStride = 0;

	D3D11_SAMPLER_DESC sampler_desc{};
	sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.MipLODBias = 0;
	sampler_desc.MaxAnisotropy = 16;
	sampler_desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	sampler_desc.BorderColor[0] = 0;
	sampler_desc.BorderColor[1] = 0;
	sampler_desc.BorderColor[2] = 0;
	sampler_desc.BorderColor[3] = 0;
	sampler_desc.MinLOD = 0;
	sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = device->CreateSamplerState(&sampler_desc, sampler_state_flame.GetAddressOf());


	{
		buffer_desc.ByteWidth = sizeof(scroll_constants);
		hr = device->CreateBuffer(&buffer_desc, nullptr, scroll_constant_buffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

	{
		buffer_desc.ByteWidth = sizeof(timer_constants);
		hr = device->CreateBuffer(&buffer_desc, nullptr, timer_constant_buffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

	{
		D3D11_INPUT_ELEMENT_DESC input_element_desc[]
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		create_vs_from_cso(device.Get(),
			"flameFont_vs.cso",
			sprite_vertex_shader_flame.GetAddressOf(),
			sprite_input_layout_flame.GetAddressOf(),
			input_element_desc,
			ARRAYSIZE(input_element_desc));
		create_ps_from_cso(device.Get(),
			"flameFont_ps.cso",
			sprite_pixel_shader_flame.GetAddressOf());
	}

	{
		D3D11_INPUT_ELEMENT_DESC input_element_desc[]
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		create_vs_from_cso(device.Get(),
			"sprite_vs.cso",
			sprite_vertex_shader_player.GetAddressOf(),
			sprite_input_layout_player.GetAddressOf(),
			input_element_desc,
			ARRAYSIZE(input_element_desc));
		create_ps_from_cso(device.Get(),
			"sprite_ps.cso",
			sprite_pixel_shader_player.GetAddressOf());
	}
}

void Player::update(DirectX::XMFLOAT2 pos, float angle)
{
	pp.pos.x += pos.x;
	pp.pos.y += pos.y;
	pp.angle += angle;
}

void Player::render(Microsoft::WRL::ComPtr<ID3D11Device> device, Microsoft::WRL::ComPtr<ID3D11DeviceContext> immediate_context, float elapsed_time)
{
	{
		scroll_constants scroll{};
		scroll.scroll_directionX.x = 0.5f;
		scroll.scroll_directionX.y = 0.0f;
		scroll.scroll_directionY.x = 0.0f;
		scroll.scroll_directionY.y = 0.5f;

		immediate_context->UpdateSubresource(scroll_constant_buffer.Get(), 0, 0, &scroll, 0, 0);
		immediate_context->VSSetConstantBuffers(1, 1, scroll_constant_buffer.GetAddressOf());
		immediate_context->PSSetConstantBuffers(1, 1, scroll_constant_buffer.GetAddressOf());

		timer_constants timer{};
		timer.parameters.x = elapsed_time;
		immediate_context->UpdateSubresource(timer_constant_buffer.Get(), 0, 0, &timer, 0, 0);
		immediate_context->VSSetConstantBuffers(2, 1, timer_constant_buffer.GetAddressOf());
		immediate_context->PSSetConstantBuffers(2, 1, timer_constant_buffer.GetAddressOf());
	}

	{
		immediate_context->IASetInputLayout(sprite_input_layout_flame.Get());
		immediate_context->VSSetShader(sprite_vertex_shader_flame.Get(), nullptr, 0);
		immediate_context->PSSetShader(sprite_pixel_shader_flame.Get(), nullptr, 0);

		immediate_context->PSSetSamplers(0, 1, sampler_state_flame.GetAddressOf());

		immediate_context->PSSetShaderResources(1, 1, mask_texture_noise1.GetAddressOf());
		immediate_context->PSSetShaderResources(2, 1, mask_texture_noise2.GetAddressOf());
		immediate_context->PSSetShaderResources(3, 1, mask_texture_flame1.GetAddressOf());
		immediate_context->PSSetShaderResources(4, 1, mask_texture_flame2.GetAddressOf());

		spriteJet->render(immediate_context.Get(), pp.pos.x + 34, pp.pos.y + 60, 20, 40, 1.0f, 1.0, 1.0f, 1.0f, 180 + pp.angle);
		spriteJet->render(immediate_context.Get(), pp.pos.x + 72, pp.pos.y + 60, 20, 40, 1.0f, 1.0, 1.0f, 1.0f, 180 + pp.angle);
	}


	{
		immediate_context->IASetInputLayout(sprite_input_layout_player.Get());
		immediate_context->VSSetShader(sprite_vertex_shader_player.Get(), nullptr, 0);
		immediate_context->PSSetShader(sprite_pixel_shader_player.Get(), nullptr, 0);

		immediate_context->PSSetSamplers(0, 1, sampler_state_flame.GetAddressOf());

		spritePlayer->render(immediate_context.Get(), pp.pos.x, pp.pos.y, 128, 128, 1.0f, 1.0f, 1.0f, 1.0f, pp.angle);
	}
}

Player::~Player()
{
}