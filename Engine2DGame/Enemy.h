#pragma once

#include "Player.h"

#define ENEMY_TEX_W            (128.0f)   
#define ENEMY_TEX_H            (128.0f)    
#define ENEMY_PIVOT_X          (64.0f)     
#define ENEMY_PIVOT_Y          (64.0f)  

class Enemy :
    public Player
{
public:
	Enemy();
	~Enemy();

	void init(Microsoft::WRL::ComPtr<ID3D11Device> device);
	void render(Microsoft::WRL::ComPtr<ID3D11Device> device, Microsoft::WRL::ComPtr<ID3D11DeviceContext> immediate_context, float elapsed_time);
	void update(DirectX::XMFLOAT2 pos, float angle);

	DirectX::XMFLOAT2 getPos() { return pp.pos; }
	float getAngle() { return pp.angle; }
	void setAct(int act) { this->pp.act = act; }

private:

	struct
	{
		int                 state;      // 状態
		int                 timer;      // タイマー

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
		int                 jumpCount;  // 残りジャンプ回数
		int                 act;        // プレイヤーの行動遷移用
		int                 anime;      // アニメが現在何コマ目か
		int                 animeTimer; // アニメ用タイマー

		float               radius;     // 半径
		DirectX::XMFLOAT2             offset;     // 位置からあたり領域の中心へのオフセット
		DirectX::XMFLOAT2             origin;     // 初期位置を保持
		float               angle;      // 角度

		int                 hp;
	} pp;

	Microsoft::WRL::ComPtr<ID3D11VertexShader> sprite_vertex_shader_enemy;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> sprite_input_layout_enemy;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> sprite_pixel_shader_enemy;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler_state_enemy;

	std::unique_ptr<sprite> spriteEnemy;
};

