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

#define PLAYER_TEX_W            (128.0f)   
#define PLAYER_TEX_H            (128.0f)    
#define PLAYER_PIVOT_X          (64.0f)     
#define PLAYER_PIVOT_Y          (128.0f)    

class Player
{
public:
	Player();
	~Player();

	void init(Microsoft::WRL::ComPtr<ID3D11Device> device);
	void render(Microsoft::WRL::ComPtr<ID3D11Device> device, Microsoft::WRL::ComPtr<ID3D11DeviceContext> immediate_context, float elapsed_time);
	Shot* searchSet(std::vector<Shot>& shots);
	void update(DirectX::XMFLOAT2 pos, float angle);

	DirectX::XMFLOAT2 getPos() { return playerProperty.pos; }
	float getAngle() { return playerProperty.angle; }
	void setAct(int act) { this->playerProperty.act = act; }

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
	} playerProperty;

	struct scroll_constants
	{
		DirectX::XMFLOAT2 scroll_directionX;
		DirectX::XMFLOAT2 scroll_directionY;
	};

	struct timer_constants
	{
		DirectX::XMFLOAT4 parameters;
	};

	Microsoft::WRL::ComPtr<ID3D11Buffer> scroll_constant_buffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> timer_constant_buffer;

	D3D11_TEXTURE2D_DESC mask_texture2dDesc;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mask_texture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mask_texture_noise1;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mask_texture_noise2;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mask_texture_flame1;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mask_texture_flame2;

	Microsoft::WRL::ComPtr<ID3D11VertexShader> sprite_vertex_shader_flame;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> sprite_input_layout_flame;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> sprite_pixel_shader_flame;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler_state_flame;

	Microsoft::WRL::ComPtr<ID3D11VertexShader> sprite_vertex_shader_player;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> sprite_input_layout_player;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> sprite_pixel_shader_player;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler_state_player;

	std::unique_ptr<sprite> spritePlayer;
	std::unique_ptr<sprite> spriteJet;
	std::unique_ptr<sprite> spriteAmmo;
	std::unique_ptr<sprite> spriteAmmoTypeOne;
	std::unique_ptr<sprite> spriteAmmoTypeTwo;

};