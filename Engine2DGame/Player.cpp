#include "Player.h"

Player::Player()
{

}

void Player::init(Microsoft::WRL::ComPtr<ID3D11Device> device, std::vector<Shot>& listOfShots)
{
	
	playerProperty.pos = { 512, 512 };
	playerProperty.angle = 0;
	playerProperty.shotCount = 0;
	playerProperty.hp = 15;
	playerProperty.act = 10;
	playerProperty.state = 1;
	playerProperty.invincible = false;

	this->listOfShots = &listOfShots;

	spritePlayer = std::make_unique<sprite>(device.Get(), L".\\resources\\player\\player2.png");
	spriteJet = std::make_unique<sprite>(device.Get(), L".\\resources\\FlameLetters\\ShapeTextureFull.png");
	spriteAmmo =  std::make_unique<sprite>(device.Get(), L".\\resources\\ammo\\shield.png");
	spriteAmmoTypeOne = std::make_unique<sprite>(device.Get(), L".\\resources\\ammo\\rocket.png");
	spriteAmmoTypeTwo = std::make_unique<sprite>(device.Get(), L".\\resources\\ammo\\shield_icon.png");
	shieldAnimation = std::make_unique<sprite>(device.Get(), L".\\resources\\ammo\\shieldAnimation\\shieldanim.png");
	

	load_texture_from_file(device.Get(), L".\\resources\\FlameLetters\\NoiseTexture1.png", mask_texture_noise1.GetAddressOf(), &mask_texture2dDesc);
	load_texture_from_file(device.Get(), L".\\resources\\FlameLetters\\NoiseTexture2.png", mask_texture_noise2.GetAddressOf(), &mask_texture2dDesc);
	load_texture_from_file(device.Get(), L".\\resources\\FlameLetters\\FlameTexture1.png", mask_texture_flame1.GetAddressOf(), &mask_texture2dDesc);
	load_texture_from_file(device.Get(), L".\\resources\\FlameLetters\\FlameTexture2.png", mask_texture_flame2.GetAddressOf(), &mask_texture2dDesc);

	life0 = std::make_unique<sprite>(device.Get(), L".\\resources\\player\\life0.png");
	life1 = std::make_unique<sprite>(device.Get(), L".\\resources\\player\\life1.png");
	life2 = std::make_unique<sprite>(device.Get(), L".\\resources\\player\\life2.png");
	life3 = std::make_unique<sprite>(device.Get(), L".\\resources\\player\\life3.png");

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

void Player::update(DirectX::XMFLOAT2 pos, float angle, int action)
{

	if (GetAsyncKeyState(0x57) < 0)
	{
		pos = { 5 * sinf(playerProperty.angle * 3.14 / 180.0f), 5 * -cosf(playerProperty.angle * 3.14 / 180.0f) };
		playerProperty.act = (2);
	}
	if (GetAsyncKeyState(0x53) < 0)
	{
		pos = { -3 * sinf(playerProperty.angle * 3.14 / 180.0f), -3 * -cosf(playerProperty.angle * 3.14 / 180.0f) };
		playerProperty.act = (3);
	}

	if (GetAsyncKeyState(0x41) < 0)
	{
		angle = -3;
	}

	if (GetAsyncKeyState(0x44) < 0)
	{
		angle = +3;
	}

	if (GetAsyncKeyState(0x51) < 0)
	{
		if (playerProperty.timer - playerProperty.oldTimerSwitch > 20 && !playerProperty.invincible)
		{
			playerProperty.state = -playerProperty.state;
			playerProperty.oldTimerSwitch = playerProperty.timer;
		}
	}

	if (GetAsyncKeyState(0x20) < 0)
	{
		if (playerProperty.state == 1)
		{
			if (playerProperty.timer - playerProperty.oldTimer > 10)
			{
				Shot* shot = searchSet(*this->listOfShots);
				shot->setAct(0);
				shot->update(getPos(), getAngle(), playerProperty.shotCount);
				playerProperty.oldTimer = playerProperty.timer;
				playerProperty.shotCount = !playerProperty.shotCount;
			}
		}

		if (playerProperty.state == -1)
		{
			if (!playerProperty.invincible)
			{
				if (playerProperty.timer - playerProperty.oldTimerShield > 100)
				{
					playerProperty.invincible = true;
					playerProperty.oldTimerShield = playerProperty.timer;
				}
			}

		}
	}

	if (playerProperty.pos.x < 30 - 64)
	{
		playerProperty.pos.x = 30 - 64;
	}
	if (playerProperty.pos.x >= 1280 - 30 - 64)
	{
		playerProperty.pos.x = 1280 - 30 - 64;
	}
	if (playerProperty.pos.y < 30 - 64)
	{
		playerProperty.pos.y = 30 - 64;
	}
	if (playerProperty.pos.y > 720 - 30 - 64)
	{
		playerProperty.pos.y = 720 - 30 - 64;
	}

	playerProperty.pos.x += pos.x;
	playerProperty.pos.y += pos.y;
	playerProperty.angle += angle;

	playerProperty.timer++;
}

void Player::render(Microsoft::WRL::ComPtr<ID3D11Device> device, Microsoft::WRL::ComPtr<ID3D11DeviceContext> immediate_context, float elapsed_time)
{
	{
		if (playerProperty.act == 0) return;

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

		float tempX1 = cosf(playerProperty.angle * 3.14 / 180) * (playerProperty.pos.x + 45 - (playerProperty.pos.x + 64)) - sinf(playerProperty.angle * 3.14 / 180) * (playerProperty.pos.y + 90 - (playerProperty.pos.y + 64)) + playerProperty.pos.x + 64;
		float tempY1 = sinf(playerProperty.angle * 3.14 / 180) * (playerProperty.pos.x + 45 - (playerProperty.pos.x + 64)) + cosf(playerProperty.angle * 3.14 / 180) * (playerProperty.pos.y + 90 - (playerProperty.pos.y + 64)) + playerProperty.pos.y + 64;
		float tempX2 = cosf(playerProperty.angle * 3.14 / 180) * (playerProperty.pos.x + 83 - (playerProperty.pos.x + 64)) - sinf(playerProperty.angle * 3.14 / 180) * (playerProperty.pos.y + 90 - (playerProperty.pos.y + 64)) + playerProperty.pos.x + 64;
		float tempY2 = sinf(playerProperty.angle * 3.14 / 180) * (playerProperty.pos.x + 83 - (playerProperty.pos.x + 64)) + cosf(playerProperty.angle * 3.14 / 180) * (playerProperty.pos.y + 90 - (playerProperty.pos.y + 64)) + playerProperty.pos.y + 64;

		switch (playerProperty.act)
		{
		case 1:
			spriteJet->render(immediate_context.Get(), tempX1 - 10, tempY1 - 20, 20, 40, 1.0f, 1.0, 1.0f, 1.0f, 180 + playerProperty.angle);
			spriteJet->render(immediate_context.Get(), tempX2 - 10, tempY2 - 20, 20, 40, 1.0f, 1.0, 1.0f, 1.0f, 180 + playerProperty.angle);
			break;
		case 2:
			spriteJet->render(immediate_context.Get(), tempX1 - 10, tempY1 - 30, 20, 60, 1.0f, 1.0, 1.0f, 1.0f, 180 + playerProperty.angle);
			spriteJet->render(immediate_context.Get(), tempX2 - 10, tempY2 - 30, 20, 60, 1.0f, 1.0, 1.0f, 1.0f, 180 + playerProperty.angle);
			break;
		case 3:
			spriteJet->render(immediate_context.Get(), tempX1 - 10, tempY1 - 15, 20, 30, 1.0f, 1.0, 1.0f, 1.0f, 180 + playerProperty.angle);
			spriteJet->render(immediate_context.Get(), tempX2 - 10, tempY2 - 15, 20, 30, 1.0f, 1.0, 1.0f, 1.0f, 180 + playerProperty.angle);
			break;
		default:
			break;
		}

	}

	{
		immediate_context->IASetInputLayout(sprite_input_layout_player.Get());
		immediate_context->VSSetShader(sprite_vertex_shader_player.Get(), nullptr, 0);
		immediate_context->PSSetShader(sprite_pixel_shader_player.Get(), nullptr, 0);

		immediate_context->PSSetSamplers(0, 1, sampler_state_flame.GetAddressOf());

		if (playerProperty.hp >= 11)
		{
			life3->render(immediate_context.Get(), 20, -10, 128, 128, 1.0f, 1.0, 1.0f, 1.0f, 0);
		}
		if (playerProperty.hp >= 6 && playerProperty.hp < 11)
		{
			life2->render(immediate_context.Get(), 20, -10, 128, 128, 1.0f, 1.0, 1.0f, 1.0f, 0);
		}
		if (playerProperty.hp >= 1 && playerProperty.hp < 6)
		{
			life1->render(immediate_context.Get(), 20, -10, 128, 128, 1.0f, 1.0, 1.0f, 1.0f, 0);
		}
		if (playerProperty.hp <= 0)
		{
			life0->render(immediate_context.Get(), 20, -10, 128, 128, 1.0f, 1.0, 1.0f, 1.0f, 0);
			playerProperty.act = 0;
		}

		spritePlayer->render(immediate_context.Get(), playerProperty.pos.x, playerProperty.pos.y, 128, 128, 1.0f, 1.0f, 1.0f, 1.0f, playerProperty.angle);

		if (playerProperty.invincible)
		{
			shieldAnimation->render(immediate_context.Get(), playerProperty.pos.x, playerProperty.pos.y, 128, 128, 1.0f, 1.0f, 1.0f, 1.0f, 0, playerProperty.animeTimer % 10 * 128.0f, 128, 128, 128);

			if (playerProperty.timer - playerProperty.oldTimerShield > 10)
			{
				playerProperty.animeTimer++;
				playerProperty.oldTimerShield = playerProperty.timer;

				if (playerProperty.animeTimer == 8)
				{
					playerProperty.invincible = false;
					playerProperty.animeTimer = 0;
				}
			}

		}

	}

	if (playerProperty.state == 1)
	{
		spriteAmmoTypeOne->render(immediate_context.Get(), 50 + 8, 650 + 8, 48, 48, 1.0f, 1.0f, 1.0f, 1.0f, 0);
		spriteAmmo->render(immediate_context.Get(), 50, 650, 64, 64, 1.0f, 1.0f, 1.0f, 1.0f, 0);
	}
	else
	{
		spriteAmmoTypeTwo->render(immediate_context.Get(), 50 + 8, 650 + 8, 48, 48, 1.0f, 1.0f, 1.0f, 1.0f, 0);
		spriteAmmo->render(immediate_context.Get(), 50, 650, 64, 64, 1.0f, 1.0f, 1.0f, 1.0f, 0);
	}
}

Shot* Player::searchSet(std::vector<Shot>& shots)
{
	for (auto p = shots.begin(); p != shots.end(); ++p)
	{
		if (p->getAct() != 10) continue;
		auto index = std::distance(shots.begin(), p);
		p->setState(index);
		return &(*p);
	}
	return NULL;
}



Player::~Player()
{
}