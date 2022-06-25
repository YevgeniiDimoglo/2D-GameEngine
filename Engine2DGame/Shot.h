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
		int                 state;      // ���
		int                 timer;      // �^�C�}�[

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
		int                 jumpCount;  // �c��W�����v��
		int                 act;        // �v���C���[�̍s���J�ڗp
		int                 anime;      // �A�j�������݉��R�}�ڂ�
		int                 animeTimer; // �A�j���p�^�C�}�[

		float               radius;     // ���a
		DirectX::XMFLOAT2             offset;     // �ʒu���炠����̈�̒��S�ւ̃I�t�Z�b�g
		DirectX::XMFLOAT2             origin;     // �����ʒu��ێ�
		float               angle;      // �p�x

		int                 hp;
	} sp;

	Microsoft::WRL::ComPtr<ID3D11VertexShader> sprite_vertex_shader_shot;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> sprite_input_layout_shot;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> sprite_pixel_shader_shot;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler_state_shot;

	std::unique_ptr<sprite> spriteShot;
};

