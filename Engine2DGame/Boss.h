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
#include "Player.h"

#define PLAYER_TEX_W            (128.0f)   
#define PLAYER_TEX_H            (128.0f)    
#define PLAYER_PIVOT_X          (64.0f)     
#define PLAYER_PIVOT_Y          (128.0f)    

class Boss
{
public:
	Boss();
	~Boss();

	void init(Microsoft::WRL::ComPtr<ID3D11Device> device);
	void render(Microsoft::WRL::ComPtr<ID3D11Device> device, Microsoft::WRL::ComPtr<ID3D11DeviceContext> immediate_context, float elapsed_time);
	void renderAttack(Microsoft::WRL::ComPtr<ID3D11DeviceContext> immediate_context);
	void updateAttack(int number);
	void update();

	int getTimer() { return bossProperty.timer; }
	void setTimer (int timer) { this->bossProperty.timer = timer; }

private:

	struct
	{
		int                 state;      // 状態
		int                 timer;      // タイマー
		int					oldTimer;
		int					oldTimerSwitch;
		int					oldTimerShield;

		bool                onGround;   // 地面フラグ
		bool                invincible;    // 着地フラグ
		char                pad[2];     // パディング（隙間）

		int                 area;       // エリア
		DirectX::XMFLOAT2             pos;        // 位置
		DirectX::XMFLOAT2             scale;      // スケール
		DirectX::XMFLOAT2             texPos;     // 画像位置
		DirectX::XMFLOAT2             texSize;    // 画像サイズ
		DirectX::XMFLOAT2             pivot;      // 基準点
		DirectX::XMFLOAT4             color;      // 色

		DirectX::XMFLOAT2             speed;      // 速度
		bool                shotCount;  // 残りジャンプ回数
		int                 act;        // プレイヤーの行動遷移用
		int                 anime;      // アニメが現在何コマ目か
		int                 animeTimer; // アニメ用タイマー

		float               radius;     // 半径
		DirectX::XMFLOAT2             offset;     // 位置からあたり領域の中心へのオフセット
		DirectX::XMFLOAT2             origin;     // 初期位置を保持
		float               angle;      // 角度

		int                 hp;
	} bossProperty;

	struct bossAttack
	{
		bool render;
		DirectX::XMFLOAT2 pos;
		int animeTimer;

	};

	D3D11_TEXTURE2D_DESC mask_texture2dDesc;

	Microsoft::WRL::ComPtr<ID3D11VertexShader> sprite_vertex_shader_boss;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> sprite_input_layout_boss;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> sprite_pixel_shader_boss;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler_state_boss;

	std::unique_ptr<sprite> spriteBoss;
	std::unique_ptr<sprite> spriteBossPylon;
	std::unique_ptr<sprite> attackAnimation;

	Player* player;
	std::vector<bossAttack> attacks { 30 };
};

