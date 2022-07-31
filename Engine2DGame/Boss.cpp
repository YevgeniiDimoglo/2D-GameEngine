#include "Boss.h"

Boss::Boss()
{
}

void Boss::init(Microsoft::WRL::ComPtr<ID3D11Device> device)
{
	this->player = player;
	bossProperty.pos = { 0, 0 };
	bossProperty.hp = 500;
	bossProperty.act = 1;
	bossProperty.timer = 0;
	bossProperty.animeTimer = 0;

	spriteBoss = std::make_unique<sprite>(device.Get(), L".\\resources\\enemy\\boss.png");
	spriteBossPylon = std::make_unique<sprite>(device.Get(), L".\\resources\\enemy\\boss2.png");
	attackAnimation = std::make_unique<sprite>(device.Get(), L".\\resources\\ammo\\bossAttackAnim\\boss_attack.png");

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
	hr = device->CreateSamplerState(&sampler_desc, sampler_state_boss.GetAddressOf());

	{
		D3D11_INPUT_ELEMENT_DESC input_element_desc[]
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		create_vs_from_cso(device.Get(),
			"sprite_vs.cso",
			sprite_vertex_shader_boss.GetAddressOf(),
			sprite_input_layout_boss.GetAddressOf(),
			input_element_desc,
			ARRAYSIZE(input_element_desc));
		create_ps_from_cso(device.Get(),
			"sprite_ps.cso",
			sprite_pixel_shader_boss.GetAddressOf());
	}

	attacks = {
		{ 0, { 50  -517, 230 -517},   0 },
		{ 0, { 1230  -517, 230 -517},   0 },

		{ 0, { 50 - 517, 230 - 517},   0 },

		{ 0, { 620  -517, 280 -517},   0 },

		{ 0, { 620 -517, 280 -517},   0 },

		{ 0, { 620 -517, 280 -517},   0 },

		{ 0, { 480 -517, 270 -517},   0 },
		{ 0, { 760  -517, 270 -517},   0 },

		{ 0, { 320  -517, 260 -517},   0 },
		{ 0, { 960  -517, 260 -517},   0 },

		{ 0, { 50  -517, 230 -517},   0 },
		{ 0, { 1230  -517, 230 -517},   0 },

		{ 0, { 620 - 517, 280 - 517},   0 },

		{ 0, { 50  -517, 230 -517},   0 },
		{ 0, { 1230  -517, 230 -517},   0 },

		{ 0, { 620 -517, 280 -517},   0 },
		{ 0, { 50 - 517, 230 - 517},   0 },

		{ 0, { 620 - 517, 280 - 517},   0 },
		{ 0, { 1230  -517, 230 -517},   0 },

		{ 0, { 1230 - 517, 230 - 517},   0 },

		{ 0, { 620 - 517, 280 - 517},   0 },

		{ 0, { 50  -517, 230 -517},   0 },

		{ 0, { 620 - 517, 280 - 517},   0 },
		{ 0, { 50  -517, 230 -517},   0 },
		{ 0, { 1230  -517, 230 -517},   0 },

		{ 0, { 620 -517, 280 -517},   0 },

		{ 0, { 620 -517, 280 -517},   0 },
	};
}

void Boss::render(Microsoft::WRL::ComPtr<ID3D11Device> device, Microsoft::WRL::ComPtr<ID3D11DeviceContext> immediate_context, float elapsed_time)
{
	immediate_context->IASetInputLayout(sprite_input_layout_boss.Get());
	immediate_context->VSSetShader(sprite_vertex_shader_boss.Get(), nullptr, 0);
	immediate_context->PSSetShader(sprite_pixel_shader_boss.Get(), nullptr, 0);

	immediate_context->PSSetSamplers(0, 1, sampler_state_boss.GetAddressOf());

	spriteBossPylon->render(immediate_context.Get(), bossProperty.pos.x + 450, bossProperty.pos.y + 100, 320, 320, 1.0f, 1.0f, 1.0f, 1.0f, 0);
	spriteBoss->render(immediate_context.Get(), bossProperty.pos.x, bossProperty.pos.y, 1280, 512, 1.0f, 1.0f, 1.0f, 1.0f, 0);
	
	if (bossProperty.timer % 1 == 0)
	{
		renderAttack(immediate_context.Get());
	}
}

void Boss::renderAttack(Microsoft::WRL::ComPtr<ID3D11DeviceContext> immediate_context)
{
	for (size_t i = 0; i < attacks.size(); i++)
	{
		if (attacks[i].render)
		{
			attacks[i].animeTimer++;

			attackAnimation->render(immediate_context.Get(), attacks[i].pos.x, attacks[i].pos.y, 1024, 1024, 1.0f, 1.0f, 1.0f, 1.0f, 0, attacks[i].animeTimer / 2 * 1034.0f, attacks[i].animeTimer / 2 / 11 * 1034.0f, 1034, 1034);

			if (attacks[i].animeTimer == 80)
			{
				attacks[i].animeTimer == 0;
				attacks[i].render = false;
			}
		}

	}
}

void Boss::updateAttack(int number)
{
	attacks[number].render = true;
}

struct PTN_DATA
{
	int timer;
	int ptn;
};

PTN_DATA bossPtn[] =
{
	{50, 0},
	{50, 1},

	{270, 2, },

	{770, 3, },

	{900, 4, },

	{1030, 5, },

	{1270, 6, },
	{1270, 7, },

	{1400, 8, },
	{1400, 9, },

	{1530, 10, },
	{1530, 11, },

	{2030, 12, },

	{2530, 13, },
	{2530, 14, },

	{2660, 15, },
	{2660, 16, },

	{2790, 17, },
	{2790, 18, },

	{3070, 19, },

	{3200, 20, },

	{3330, 21, },

	{4000, 22, },
	{4000, 23, },
	{4000, 24, },

	{4500, 25, },

	{-1, -1}
};

void Boss::update()
{
	bossProperty.timer++;

	for (int n = 0; bossPtn[n].ptn != -1; ++n)
	{
		if (bossProperty.timer == bossPtn[n].timer)
		{
			updateAttack(bossPtn[n].ptn);
		}
	}
}

Boss::~Boss()
{
}