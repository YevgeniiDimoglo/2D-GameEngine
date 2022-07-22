#include "sprite.h"
#include "misc.h"

#include <sstream>

#include <functional>

#include "texture.h"
#include "shader.h"

sprite::sprite(ID3D11Device* device, const wchar_t* filename)
{
	HRESULT hr{ S_OK };

	vertex vertices[]
	{
		{ { -1.0, +1.0, 0 }, { 1, 1, 1, 1 }, { 0, 0 } },
		{ { +1.0, +1.0, 0 }, { 1, 1, 1, 1 }, { 1, 0 } },
		{ { -1.0, -1.0, 0 }, { 1, 1, 1, 1 }, { 0, 1 } },
		{ { +1.0, -1.0, 0 }, { 1, 1, 1, 1 }, { 1, 1 } },
	};

	D3D11_BUFFER_DESC buffer_desc{};
	buffer_desc.ByteWidth = sizeof(vertices);
	buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
	buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	buffer_desc.MiscFlags = 0;
	buffer_desc.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA subresource_data{};
	subresource_data.pSysMem = vertices;
	subresource_data.SysMemPitch = 0;
	subresource_data.SysMemSlicePitch = 0;
	hr = device->CreateBuffer(&buffer_desc, &subresource_data, vertex_buffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	{
		D3D11_INPUT_ELEMENT_DESC input_element_desc[]
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		create_vs_from_cso(device,
			"sprite_vs.cso",
			vertexShader.GetAddressOf(),
			inputLayout.GetAddressOf(),
			input_element_desc,
			ARRAYSIZE(input_element_desc));
		create_ps_from_cso(device,
			"sprite_ps.cso",
			pixelShader.GetAddressOf());
	}

	D3D11_RASTERIZER_DESC rsDesc = {};
	rsDesc.FillMode = D3D11_FILL_SOLID;//
	rsDesc.CullMode = D3D11_CULL_NONE;//	カリング
	rsDesc.FrontCounterClockwise = false;
	rsDesc.DepthBias = 0;
	rsDesc.DepthBiasClamp = 0;
	rsDesc.SlopeScaledDepthBias = 0;
	rsDesc.DepthClipEnable = false;
	rsDesc.ScissorEnable = false;
	rsDesc.MultisampleEnable = false;
	rsDesc.AntialiasedLineEnable = false;
	if (FAILED(device->CreateRasterizerState(&rsDesc, &rasterizerState)))
	{
		assert(!"ラスタライザステートの作成に失敗(Primitive)");
		return;
	}

	D3D11_SAMPLER_DESC sampler_desc{};
	sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
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
	hr = device->CreateSamplerState(&sampler_desc, samplerState.GetAddressOf());

	load_texture_from_file(device, filename, shader_resource_view.GetAddressOf(), &texture2d_desc);
}
void sprite::render(ID3D11DeviceContext* immediate_context,
	float dx, float dy, float dw, float dh,
	float r, float g, float b, float a,
	float angle/*degree*/)
{
	render(immediate_context, dx, dy, dw, dh, r, g, b, a, angle, 0.0f, 0.0f, static_cast<float>(texture2d_desc.Width), static_cast<float>(texture2d_desc.Height));
}
void sprite::render(ID3D11DeviceContext* immediate_context,
	float dx, float dy, float dw, float dh,
	float r, float g, float b, float a,
	float angle/*degree*/,
	float sx, float sy, float sw, float sh)
{
	D3D11_VIEWPORT viewport{};
	UINT num_viewports{ 1 };
	immediate_context->RSGetViewports(&num_viewports, &viewport);

	// Set each sprite's vertices coordinate to screen space
	//
	//  (x0, y0) *----* (x1, y1) 
	//	         |   /|
	//	         |  / |
	//	         | /  |
	//	         |/   |
	//  (x2, y2) *----* (x3, y3) 

	// left-top
	float x0{ dx };
	float y0{ dy };
	// right-top
	float x1{ dx + dw };
	float y1{ dy };
	// left-bottom
	float x2{ dx };
	float y2{ dy + dh };
	// right-bottom
	float x3{ dx + dw };
	float y3{ dy + dh };

	//auto rotate = [](float& x, float& y, float cx, float cy, float angle)
	std::function<void(float&, float&, float, float, float)> rotate = [](float& x, float& y, float cx, float cy, float angle)
	{
		x -= cx;
		y -= cy;

		float cos{ cosf(DirectX::XMConvertToRadians(angle)) };
		float sin{ sinf(DirectX::XMConvertToRadians(angle)) };
		float tx{ x }, ty{ y };
		x = cos * tx + -sin * ty;
		y = sin * tx + cos * ty;

		x += cx;
		y += cy;
	};
	float cx = dx + dw * 0.5f;
	float cy = dy + dh * 0.5f;
	rotate(x0, y0, cx, cy, angle);
	rotate(x1, y1, cx, cy, angle);
	rotate(x2, y2, cx, cy, angle);
	rotate(x3, y3, cx, cy, angle);

	// Convert to NDC space
	x0 = 2.0f * x0 / viewport.Width - 1.0f;
	y0 = 1.0f - 2.0f * y0 / viewport.Height;
	x1 = 2.0f * x1 / viewport.Width - 1.0f;
	y1 = 1.0f - 2.0f * y1 / viewport.Height;
	x2 = 2.0f * x2 / viewport.Width - 1.0f;
	y2 = 1.0f - 2.0f * y2 / viewport.Height;
	x3 = 2.0f * x3 / viewport.Width - 1.0f;
	y3 = 1.0f - 2.0f * y3 / viewport.Height;

	HRESULT hr{ S_OK };
	D3D11_MAPPED_SUBRESOURCE mapped_subresource{};
	hr = immediate_context->Map(vertex_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_subresource);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	vertex* vertices{ reinterpret_cast<vertex*>(mapped_subresource.pData) };
	if (vertices != nullptr)
	{
		vertices[0].position = { x0, y0 , 0 };
		vertices[1].position = { x1, y1 , 0 };
		vertices[2].position = { x2, y2 , 0 };
		vertices[3].position = { x3, y3 , 0 };

		vertices[0].color = vertices[1].color = vertices[2].color = vertices[3].color = { r, g, b, a };

		vertices[0].texcoord = { sx / texture2d_desc.Width, sy / texture2d_desc.Height };
		vertices[1].texcoord = { (sx + sw) / texture2d_desc.Width, sy / texture2d_desc.Height };
		vertices[2].texcoord = { sx / texture2d_desc.Width, (sy + sh) / texture2d_desc.Height };
		vertices[3].texcoord = { (sx + sw) / texture2d_desc.Width, (sy + sh) / texture2d_desc.Height };
	}

	immediate_context->Unmap(vertex_buffer.Get(), 0);

	UINT stride{ sizeof(vertex) };
	UINT offset{ 0 };
	immediate_context->IASetVertexBuffers(0, 1, vertex_buffer.GetAddressOf(), &stride, &offset);

	immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	immediate_context->PSSetShaderResources(0, 1, shader_resource_view.GetAddressOf());

	immediate_context->Draw(4, 0);
}
void sprite::render(ID3D11DeviceContext* immediate_context, float dx, float dy, float dw, float dh)
{
	render(immediate_context, dx, dy, dw, dh, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
		0.0f, 0.0f, static_cast<float>(texture2d_desc.Width), static_cast<float>(texture2d_desc.Height));
}

void sprite::render(ID3D11DeviceContext* immediate_context,
	const DirectX::XMFLOAT2& position, const DirectX::XMFLOAT2& scale,
	const DirectX::XMFLOAT2& texPos, const DirectX::XMFLOAT2& texSize,
	const DirectX::XMFLOAT2& center, float angle,
	const DirectX::XMFLOAT4& color) const
{
	if (scale.x == 0.0f || scale.y == 0.0f) return;

	D3D11_VIEWPORT viewport;
	UINT numViewports = 1;
	immediate_context->RSGetViewports(&numViewports, &viewport);

	float tw = texSize.x;
	float th = texSize.y;
	if (tw == FLT_MIN && th == FLT_MIN)
	{
		tw = static_cast<float>(texture2d_desc.Width);
		th = static_cast<float>(texture2d_desc.Height);
	}

	vertex vertices[4] = {};
#ifdef GAMELIB_PLUS_UP
	vertices[0] = { VECTOR3(0.0f, 1.0f, 0), color, VECTOR2(0, 0) };
	vertices[1] = { VECTOR3(1.0f, 1.0f, 0), color, VECTOR2(1, 0) };
	vertices[2] = { VECTOR3(0.0f, 0.0f, 0), color, VECTOR2(0, 1) };
	vertices[3] = { VECTOR3(1.0f, 0.0f, 0), color, VECTOR2(1, 1) };
#else
	vertices[0] = { DirectX::XMFLOAT3(0.0f, 0.0f, 0), color, DirectX::XMFLOAT2(0, 0) };
	vertices[1] = { DirectX::XMFLOAT3(1.0f, 0.0f, 0), color, DirectX::XMFLOAT2(1, 0) };
	vertices[2] = { DirectX::XMFLOAT3(0.0f, 1.0f, 0), color, DirectX::XMFLOAT2(0, 1) };
	vertices[3] = { DirectX::XMFLOAT3(1.0f, 1.0f, 0), color, DirectX::XMFLOAT2(1, 1) };
#endif

	float sinValue = sinf(angle);
	float cosValue = cosf(angle);
	float mx = (tw * scale.x) / tw * center.x;
	float my = (th * scale.y) / th * center.y;
	for (int i = 0; i < 4; i++)
	{
		vertices[i].position.x *= (tw * scale.x);
		vertices[i].position.y *= (th * scale.y);

		vertices[i].position.x -= mx;
		vertices[i].position.y -= my;

		float rx = vertices[i].position.x;
		float ry = vertices[i].position.y;
		vertices[i].position.x = rx * cosValue - ry * sinValue;
		vertices[i].position.y = rx * sinValue + ry * cosValue;

		vertices[i].position.x += mx;
		vertices[i].position.y += my;

		vertices[i].position.x += (position.x - scale.x * center.x);
		vertices[i].position.y += (position.y - scale.y * center.y);

		vertices[i].position.x = -1.0f + vertices[i].position.x * 2 / viewport.Width;
		vertices[i].position.y = 1.0f - vertices[i].position.y * 2 / viewport.Height;
#ifdef GAMELIB_PLUS_UP
		vertices[i].position.y = -vertices[i].position.y;
#endif

		// UV座標の調整
		vertices[i].texcoord.x = (std::min)(vertices[i].texcoord.x, 1.0f);
		vertices[i].texcoord.y = (std::min)(vertices[i].texcoord.y, 1.0f);

		vertices[i].texcoord.x = (texPos.x + vertices[i].texcoord.x * tw) / texture2d_desc.Width;
		vertices[i].texcoord.y = (texPos.y + vertices[i].texcoord.y * th) / texture2d_desc.Height;
	}

	D3D11_MAPPED_SUBRESOURCE msr;
	immediate_context->Map(vertex_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
	memcpy(msr.pData, vertices, sizeof(vertices));
	immediate_context->Unmap(vertex_buffer.Get(), 0);

	UINT stride = sizeof(vertex);
	UINT offset = 0;
	immediate_context->IASetVertexBuffers(0, 1, vertex_buffer.GetAddressOf(), &stride, &offset);
	immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	immediate_context->IASetInputLayout(inputLayout.Get());
	immediate_context->RSSetState(rasterizerState.Get());
	immediate_context->VSSetShader(vertexShader.Get(), nullptr, 0);
	immediate_context->PSSetShader(pixelShader.Get(), nullptr, 0);

	immediate_context->PSSetShaderResources(0, 1, shader_resource_view.GetAddressOf());
	immediate_context->PSSetSamplers(0, 1, samplerState.GetAddressOf());

	//context->OMSetDepthStencilState(depthStencilState, 1);

	immediate_context->Draw(4, 0);
}

void sprite::circle(ID3D11DeviceContext* immediate_context,
	const DirectX::XMFLOAT2& center, float radius,
	const DirectX::XMFLOAT2& scale, float angle,
	const DirectX::XMFLOAT4& color, int n
)
{
	if (n < 3 || radius <= 0.0f) return;
	if (n > 64) n = 64;//最大64角形

	D3D11_VIEWPORT viewport;
	UINT numViewports = 1;
	immediate_context->RSGetViewports(&numViewports, &viewport);

	vertex vertices[130] = { DirectX::XMFLOAT3(0,0,0) };
	float arc = DirectX::XM_PI * 2 / n;
	vertex* p = &vertices[0];

	float sinValue = sinf(angle);
	float cosValue = cosf(angle);
	for (int i = 0; i <= n; i++)
	{
		float rx, ry;

		rx = cosf(arc * -i) * radius * scale.x;
		ry = sinf(arc * -i) * radius * scale.y;

		p->position.x = center.x + rx * cosValue - ry * sinValue;
		p->position.y = center.y + rx * sinValue + ry * cosValue;

		p->position.x = -1.0f + p->position.x * 2 / viewport.Width;

		p->position.y = 1.0f - p->position.y * 2 / viewport.Height;

		p->color = color;
		p++;

		p->position.x = center.x;
		p->position.y = center.y;

		p->position.x = -1.0f + p->position.x * 2 / viewport.Width;

		p->position.y = 1.0f - p->position.y * 2 / viewport.Height;

		p->color = color;
		p++;
	}

	D3D11_MAPPED_SUBRESOURCE msr;
	immediate_context->Map(vertex_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
	memcpy(msr.pData, vertices, sizeof(vertex) * (n + 1) * 2);
	immediate_context->Unmap(vertex_buffer.Get(), 0);

	UINT stride = sizeof(vertex);
	UINT offset = 0;
	immediate_context->IASetVertexBuffers(0, 1, vertex_buffer.GetAddressOf(), &stride, &offset);
	immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	immediate_context->IASetInputLayout(inputLayout.Get());
	immediate_context->RSSetState(rasterizerState.Get());
	immediate_context->VSSetShader(vertexShader.Get(), NULL, 0);
	immediate_context->PSSetShader(pixelShader.Get(), NULL, 0);
	//context->OMSetDepthStencilState(depthStencilState, 1);

	immediate_context->Draw((n + 1) * 2 - 1, 0);
}


void sprite::textout(ID3D11DeviceContext* immediate_context, std::string s, float x, float y, float w, float h, float r, float g, float b, float a)
{
	float sw = static_cast<float>(texture2d_desc.Width / 16);
	float sh = static_cast<float>(texture2d_desc.Height / 16);
	float carriage = 0;
	for (const char c : s)
	{
		LONG sx = c % 0x0F;
		render(immediate_context, x + carriage, y, w, h, r, g, b, a, 0, sw * (c & 0x0F), sh * (c >> 4), sw, sh);
		carriage += w;
	}
}

void sprite::textout2(ID3D11DeviceContext* immediate_context, std::string s, float x, float y, float w, float h, float r, float g, float b, float a)
{
	float sw = 30;
	float sh = 32;
	float carriage = 0;
	for (const char c : s)
	{
		LONG sx = c % 0x0F;
		render(immediate_context, x + carriage, y, w, h, r, g, b, a, 0, sw * (c - 65), 0, sw, sh);
		carriage += w;
	}
}


sprite::~sprite()
{
}