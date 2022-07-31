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
		int                 state;      // ���
		int                 timer;      // �^�C�}�[
		int					oldTimer;
		int					oldTimerSwitch;
		int					oldTimerShield;

		bool                onGround;   // �n�ʃt���O
		bool                invincible;    // ���n�t���O
		char                pad[2];     // �p�f�B���O�i���ԁj

		int                 area;       // �G���A
		DirectX::XMFLOAT2             pos;        // �ʒu
		DirectX::XMFLOAT2             scale;      // �X�P�[��
		DirectX::XMFLOAT2             texPos;     // �摜�ʒu
		DirectX::XMFLOAT2             texSize;    // �摜�T�C�Y
		DirectX::XMFLOAT2             pivot;      // ��_
		DirectX::XMFLOAT4             color;      // �F

		DirectX::XMFLOAT2             speed;      // ���x
		bool                shotCount;  // �c��W�����v��
		int                 act;        // �v���C���[�̍s���J�ڗp
		int                 anime;      // �A�j�������݉��R�}�ڂ�
		int                 animeTimer; // �A�j���p�^�C�}�[

		float               radius;     // ���a
		DirectX::XMFLOAT2             offset;     // �ʒu���炠����̈�̒��S�ւ̃I�t�Z�b�g
		DirectX::XMFLOAT2             origin;     // �����ʒu��ێ�
		float               angle;      // �p�x

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

