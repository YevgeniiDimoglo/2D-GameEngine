#pragma once

#include <windows.h>
#include <tchar.h>
#include <sstream>
#include <vector>

#include <d3d11.h>
#include <wrl.h>
#include <WICTextureLoader.h>

#include "misc.h"
#include "high_resolution_timer.h"
#include "sprite.h"
#include "shader.h"
#include "texture.h"
#include "Shot.h"
#include "Enemy.h"

bool hitCheckCircle(DirectX::XMFLOAT2 pos1, float r1, DirectX::XMFLOAT2 pos2, float r2);
bool hitCheck(Enemy* enemy, Shot* shot);
bool hitCheck(Player* player, Shot* shot);
void judge(std::vector<Shot>& listOfShots, std::vector<Enemy>& listOfEnemies);
void judge2();


