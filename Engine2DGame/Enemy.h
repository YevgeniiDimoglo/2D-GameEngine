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
		int                 state;      // ���
		int                 timer;      // �^�C�}�[
		int                 oldTimer;

		bool                onGround;   // �n�ʃt���O
		bool                jumpEnd;    // ���n�t���O
		char                pad[2];     // �p�f�B���O�i���ԁj

		int                 area;       // �G���A
		DirectX::XMFLOAT2             pos;        // �ʒu
		DirectX::XMFLOAT2             scale;      // �X�P�[��
		DirectX::XMFLOAT2             texPos;     // �摜�ʒu
		DirectX::XMFLOAT2             texSize;    // �摜�T�C�Y
		DirectX::XMFLOAT2             pivot;      // ��_
		DirectX::XMFLOAT4             color;      // �F

		DirectX::XMFLOAT2             speed;      // ���x
		DirectX::XMFLOAT2             randomLocation;      // ���x
		
		int                 act;        // �v���C���[�̍s���J�ڗp
		int                 anime;      // �A�j�������݉��R�}�ڂ�
		int                 animeTimer; // �A�j���p�^�C�}�[

		float               radius;     // ���a
		DirectX::XMFLOAT2             offset;     // �ʒu���炠����̈�̒��S�ւ̃I�t�Z�b�g
		DirectX::XMFLOAT2             origin;     // �����ʒu��ێ�
		float               angle;      // �p�x

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

