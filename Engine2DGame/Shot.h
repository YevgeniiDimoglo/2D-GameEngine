#pragma once

#include <windows.h>
#include <tchar.h>
#include <sstream>

#include <d3d11.h>
#include <wrl.h>
#include <WICTextureLoader.h>

#include "misc.h"
#include "high_resolution_timer.h"
#include "sprite.h"
#include "shader.h"
#include "texture.h"

#define SHOT_TEX_W            (32.0f)   
#define SHOT_TEX_H            (32.0f)    
#define SHOT_PIVOT_X          (16.0f)     
#define SHOT_PIVOT_Y          (16.0f)    

class Shot
{
public:
	Shot();
	~Shot();

	void init(Microsoft::WRL::ComPtr<ID3D11Device> device);
	void render(Microsoft::WRL::ComPtr<ID3D11Device> device, Microsoft::WRL::ComPtr<ID3D11DeviceContext> immediate_context, float elapsed_time);
	void update(DirectX::XMFLOAT2 pos, float angle);

	DirectX::XMFLOAT2 getPos() { return sp.pos; }
	float getAngle() { return sp.angle; }
	const int getAct() { return sp.act; }
	const int getState() { return sp.state; }
	void setAct(int act) { this->sp.act = act; }
	void setState(int state) { this->sp.state = state; }

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
	} sp;

	Microsoft::WRL::ComPtr<ID3D11VertexShader> sprite_vertex_shader_shot;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> sprite_input_layout_shot;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> sprite_pixel_shader_shot;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler_state_shot;

	std::unique_ptr<sprite> spriteShot;
};

