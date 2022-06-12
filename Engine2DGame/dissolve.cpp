#include "dissolve.h"

DissolveShader::DissolveShader()
{
}


void DissolveShader::init(Microsoft::WRL::ComPtr<ID3D11Device> device)
{
	background = std::make_unique<sprite>(device.Get(), L".\\resources\\background_pic_crop.jpg");

	load_texture_from_file(device.Get(), L".\\resources\\dissolve\\dissolve_animation.png", mask_texture_noise1.GetAddressOf(), &mask_texture2dDesc);

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
	hr = device->CreateSamplerState(&sampler_desc, sampler_state_dissolve.GetAddressOf());

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
			"dissolve_shader_vs.cso",
			sprite_vertex_shader_dissolve.GetAddressOf(),
			sprite_input_layout_dissolve.GetAddressOf(),
			input_element_desc,
			ARRAYSIZE(input_element_desc));
		create_ps_from_cso(device.Get(),
			"dissolve_shader_ps.cso",
			sprite_pixel_shader_dissolve.GetAddressOf());
	}
}

void DissolveShader::render(Microsoft::WRL::ComPtr<ID3D11Device> device, Microsoft::WRL::ComPtr<ID3D11DeviceContext> immediate_context, float elapsed_time)
{

	timer_constants timer{};
	timer.parameters.x = elapsed_time - 5;
	immediate_context->UpdateSubresource(timer_constant_buffer.Get(), 0, 0, &timer, 0, 0);
	immediate_context->VSSetConstantBuffers(1, 1, timer_constant_buffer.GetAddressOf());
	immediate_context->PSSetConstantBuffers(1, 1, timer_constant_buffer.GetAddressOf());

	if (elapsed_time > 5)
	{
		immediate_context->IASetInputLayout(sprite_input_layout_dissolve.Get());
		immediate_context->VSSetShader(sprite_vertex_shader_dissolve.Get(), nullptr, 0);
		immediate_context->PSSetShader(sprite_pixel_shader_dissolve.Get(), nullptr, 0);
		immediate_context->PSSetSamplers(0, 1, sampler_state_dissolve.GetAddressOf());
		immediate_context->PSSetShaderResources(1, 1, mask_texture_noise1.GetAddressOf());


		background->render(immediate_context.Get(), 0, 0, 1280, 720);
	}
}

DissolveShader::~DissolveShader()
{
}