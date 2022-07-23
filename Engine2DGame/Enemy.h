#pragma once

#include "Player.h"

#define ENEMY_TEX_W            (128.0f)   
#define ENEMY_TEX_H            (128.0f)    
#define ENEMY_PIVOT_X          (64.0f)     
#define ENEMY_PIVOT_Y          (64.0f)  

class Enemy 
{
public:
	Enemy();
	~Enemy();

	void init(Microsoft::WRL::ComPtr<ID3D11Device> device, Player* player);
	void render(Microsoft::WRL::ComPtr<ID3D11Device> device, Microsoft::WRL::ComPtr<ID3D11DeviceContext> immediate_context, float elapsed_time);
	void update(DirectX::XMFLOAT2 pos, float angle, std::vector<Shot>& listOfShots);
	Shot* searchSet(std::vector<Shot>& shots);
	float calculateAngle(float posX, float posY, float posX2, float PosY2);
	float calculateDistance(float posX, float posX2, float posY, float posY2);

	DirectX::XMFLOAT2 getPos() { return enemyProperty.pos; }
	float getAngle() { return enemyProperty.angle; }
	int getAct() { return enemyProperty.act; }
	int getState() { return enemyProperty.state; }
	int getHP() { return enemyProperty.hp; }
	void setAct(int act) { this->enemyProperty.act = act; }
	void setState(int state) { this->enemyProperty.state = state; }
	void setHP(int HP) { this->enemyProperty.hp = HP; }

private:

	struct
	{
		int                 state;      // 状態
		int                 timer;      // タイマー
		int                 oldTimer;

		bool                onGround;   // 地面フラグ
		bool                jumpEnd;    // 着地フラグ
		char                pad[2];     // パディング（隙間）

		int                 area;       // エリア
		DirectX::XMFLOAT2             pos;        // 位置
		DirectX::XMFLOAT2             scale;      // スケール
		DirectX::XMFLOAT2             texPos;     // 画像位置
		DirectX::XMFLOAT2             texSize;    // 画像サイズ
		DirectX::XMFLOAT2             pivot;      // 基準点
		DirectX::XMFLOAT4             color;      // 色

		DirectX::XMFLOAT2             speed;      // 速度
		DirectX::XMFLOAT2             randomLocation;      // 速度
		
		int                 act;        // プレイヤーの行動遷移用
		int                 anime;      // アニメが現在何コマ目か
		int                 animeTimer; // アニメ用タイマー

		float               radius;     // 半径
		DirectX::XMFLOAT2             offset;     // 位置からあたり領域の中心へのオフセット
		DirectX::XMFLOAT2             origin;     // 初期位置を保持
		float               angle;      // 角度

		int                 hp;
	} enemyProperty;

	Microsoft::WRL::ComPtr<ID3D11VertexShader> sprite_vertex_shader_enemy;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> sprite_input_layout_enemy;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> sprite_pixel_shader_enemy;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler_state_enemy;

	std::unique_ptr<sprite> spriteEnemy;
	std::unique_ptr<sprite> deathAnimation;

	Player* player;
};

