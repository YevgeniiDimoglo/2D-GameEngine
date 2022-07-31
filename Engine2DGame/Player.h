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

	void init(Microsoft::WRL::ComPtr<ID3D11Device> device, std::vector<Shot>& listOfShots);
	void render(Microsoft::WRL::ComPtr<ID3D11Device> device, Microsoft::WRL::ComPtr<ID3D11DeviceContext> immediate_context, float elapsed_time);
	Shot* searchSet(std::vector<Shot>& shots);
	void update(DirectX::XMFLOAT2 pos, float angle, int action);

	DirectX::XMFLOAT2 getPos() { return playerProperty.pos; }
	void setPos(DirectX::XMFLOAT2 pos) { this->playerProperty.pos = pos; }
	float getAngle() { return playerProperty.angle; }
	void setAngle( float angle) { this->playerProperty.angle = angle; }
	int getAct() { return playerProperty.act; }
	void setAct(int act) { this->playerProperty.act = act; }
	int getHP() { return playerProperty.hp; }
	void setHP(int hp) { this->playerProperty.hp = hp; }
	int getState() { return playerProperty.state; }
	void setState(int state) { this->playerProperty.state = state; }
	bool getInv() { return playerProperty.invincible; }
	void setArea(int area) { this->playerProperty.area = area; }

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
	std::unique_ptr<sprite> shieldAnimation;

	std::unique_ptr<sprite> life0;
	std::unique_ptr<sprite> life1;
	std::unique_ptr<sprite> life2;
	std::unique_ptr<sprite> life3;

	std::vector<Shot>* listOfShots;

};