#include "Enemy.h"

Enemy::Enemy()
{

}

void Enemy::init(Microsoft::WRL::ComPtr<ID3D11Device> device, Player* player)
{
	this->player = player;
	enemyProperty.pos = { 9999, 9999 };
	enemyProperty.angle = 90;
	enemyProperty.hp = 5;
	enemyProperty.radius = 32;
	enemyProperty.act = 1;
	enemyProperty.timer = 0;
	enemyProperty.animeTimer = 0;
	enemyProperty.randomLocation = {400, 400};

	spriteEnemy = std::make_unique<sprite>(device.Get(), L".\\resources\\enemy\\enemy.png");
	deathAnimation = std::make_unique<sprite>(device.Get(), L".\\resources\\ammo\\bomb.png");

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
	hr = device->CreateSamplerState(&sampler_desc, sampler_state_enemy.GetAddressOf());

	{
		D3D11_INPUT_ELEMENT_DESC input_element_desc[]
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		create_vs_from_cso(device.Get(),
			"sprite_vs.cso",
			sprite_vertex_shader_enemy.GetAddressOf(),
			sprite_input_layout_enemy.GetAddressOf(),
			input_element_desc,
			ARRAYSIZE(input_element_desc));
		create_ps_from_cso(device.Get(),
			"sprite_ps.cso",
			sprite_pixel_shader_enemy.GetAddressOf());
	}

	float distance = rand() % 300 + 800;
	float randomangle = rand() % 360;
	enemyProperty.pos.x = 540 + cos(randomangle) * distance;
	enemyProperty.pos.y = 360 + sin(randomangle) * distance;
}

void Enemy::update(std::vector<Shot>& listOfShots)
{
	if (enemyProperty.act == 0) return;

	if (enemyProperty.hp < 0)
	{
		enemyProperty.act = 9;
	}

	float angleToPlayer = calculateAngle(player->getPos().x + 60, enemyProperty.pos.x + 28, enemyProperty.pos.y + 28, player->getPos().y + 60);
	float angleToPlayer2 = angleToPlayer;
	int change = 0;

	if (angleToPlayer < 0)
	{
		angleToPlayer2 = 360 + angleToPlayer;
	}

	if (calculateDistance(player->getPos().x + 60, enemyProperty.pos.x + 28, player->getPos().y + 60, enemyProperty.pos.y + 28) >= 500)
	{
		if (enemyProperty.timer % 500 == 0)
		{
			enemyProperty.randomLocation = { (540 + (rand()% 600 - 300)) * 1.0f, (360 + (rand() % 400 - 200)) * 1.0f };
		}

		float angleToPosition = calculateAngle(enemyProperty.randomLocation.x, enemyProperty.pos.x + 28, enemyProperty.pos.y + 28, enemyProperty.randomLocation.y);

		if (angleToPosition < 0)
		{
			angleToPosition = 360 + angleToPosition;
		}

		if (calculateDistance(enemyProperty.pos.x + 28, enemyProperty.randomLocation.x, enemyProperty.pos.y+ 28, enemyProperty.randomLocation.y) >= 10)
		{
			if (abs(enemyProperty.angle - angleToPosition) > 3)
			{
				if ((enemyProperty.angle - angleToPosition) < 0)
				{
					change++;
				}
				if ((enemyProperty.angle - angleToPosition) > 0)
				{
					change--;
				}

				if (abs(enemyProperty.angle - angleToPosition) > 180)
				{
					change = -change;
				}

				enemyProperty.angle += change;
			}
			else
			{
				enemyProperty.pos.x += sinf(angleToPosition * 3.14 / 180) * 1;
				enemyProperty.pos.y += -cosf(angleToPosition * 3.14 / 180) * 1;
			}
		}

	}

	if (calculateDistance(player->getPos().x + 60, enemyProperty.pos.x + 28, player->getPos().y + 60, enemyProperty.pos.y + 28) < 500)
	{

		if (abs(enemyProperty.angle - angleToPlayer2) > 3)
		{
			if ((enemyProperty.angle - angleToPlayer2) < 0)
			{
				change++;
			}
			if ((enemyProperty.angle - angleToPlayer2) > 0)
			{
				change--;
			}

			if (abs(enemyProperty.angle - angleToPlayer2) > 180)
			{
				change = -change;
			}

			enemyProperty.angle += change;
		}
		else
		{
			if (calculateDistance(player->getPos().x + 60, enemyProperty.pos.x + 28, player->getPos().y + 60, enemyProperty.pos.y + 28) >= 300)
			{
				enemyProperty.pos.x += sinf(angleToPlayer2 * 3.14 / 180) * 1;
				enemyProperty.pos.y += -cosf(angleToPlayer2 * 3.14 / 180) * 1;
			}
		}

	}

	if (calculateDistance(player->getPos().x + 60, enemyProperty.pos.x + 28, player->getPos().y + 60, enemyProperty.pos.y + 28) < 300)
	{
		if (abs(enemyProperty.angle - angleToPlayer2) < 3)
		{
			if (enemyProperty.timer % 10 == 0)
			{
				if (enemyProperty.timer - enemyProperty.oldTimer > 10)
				{
					Shot* shot = searchSet(listOfShots);
					shot->setAct(0);
					shot->updateEnemy(enemyProperty.pos, angleToPlayer);
					enemyProperty.oldTimer = enemyProperty.timer;
				}
			}
		}
	}

	if (enemyProperty.angle > 360)
	{
		enemyProperty.angle = 0;
	}
	if (enemyProperty.angle < 0)
	{
		enemyProperty.angle = 360;
	}
	
	enemyProperty.timer += rand()%3;
}

void Enemy::render(Microsoft::WRL::ComPtr<ID3D11Device> device, Microsoft::WRL::ComPtr<ID3D11DeviceContext> immediate_context, float elapsed_time)
{
	if (enemyProperty.act == 0) return;

	if (enemyProperty.act == 9)
	{
		deathAnimation->render(immediate_context.Get(), enemyProperty.pos.x - 32, enemyProperty.pos.y - 32, 128, 128, 1.0f, 1.0f, 1.0f, 1.0f, 0, enemyProperty.animeTimer % 10 * 256.0f, enemyProperty.animeTimer / 10 * 256.0f, 256, 256);
		enemyProperty.animeTimer++;
		if (enemyProperty.animeTimer == 40)
		{
			enemyProperty.act = 0;
		}
	}
	else
	{
		immediate_context->IASetInputLayout(sprite_input_layout_enemy.Get());
		immediate_context->VSSetShader(sprite_vertex_shader_enemy.Get(), nullptr, 0);
		immediate_context->PSSetShader(sprite_pixel_shader_enemy.Get(), nullptr, 0);

		immediate_context->PSSetSamplers(0, 1, sampler_state_enemy.GetAddressOf());

		spriteEnemy->render(immediate_context.Get(), enemyProperty.pos.x, enemyProperty.pos.y, 64, 64, 1.0f, 1.0f, 1.0f, 1.0f, enemyProperty.angle);

	}

}

float Enemy::calculateDistance(float posX, float posX2, float posY, float posY2)
{
	return sqrt(pow((posX - posX2),2) + pow((posY - posY2), 2));
}

float Enemy::calculateAngle(float posX, float posX2, float posY, float posY2)
{
	return (atan2(posX - posX2, posY - posY2)) / 3.14 * 180;
}

Shot* Enemy::searchSet(std::vector<Shot>& shots)
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

Enemy::~Enemy()
{
}