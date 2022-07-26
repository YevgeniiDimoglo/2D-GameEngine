#pragma once

#include <windows.h>
#include <tchar.h>
#include <sstream>
#include <vector>

#include <d3d11.h>
#include <wrl.h>
#include "DirectXTK-master/Inc/WICTextureLoader.h"
#include "Audio.h"

#include "misc.h"
#include "high_resolution_timer.h"
#include "sprite.h"
#include "shader.h"

#include "FlameFont.h"
#include "dissolve.h"
#include "Player.h"
#include "Cloud.h"
#include "Enemy.h"
#include "Shot.h"
#include "Judge.h"
#include "Boss.h"

#ifdef USE_IMGUI
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
extern ImWchar glyphRangesJapanese[];
#endif

CONST LONG SCREEN_WIDTH{ 1280 };
CONST LONG SCREEN_HEIGHT{ 720 };
CONST BOOL FULLSCREEN{ FALSE };
CONST LPCWSTR APPLICATION_NAME{ L"X3DGP" };
CONST INT MAX_COUNT{ 512 };

#define PI  3.14159265358979323846f

class framework
{
public:
	CONST HWND hwnd;

	std::unique_ptr<DirectX::AudioEngine> m_audEngine;
	std::unique_ptr<DirectX::SoundEffect> m_explode;
	std::unique_ptr<DirectX::SoundEffect> m_ambient;
	std::unique_ptr<DirectX::SoundEffectInstance> m_nightLoop;

	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> immediate_context;
	Microsoft::WRL::ComPtr<IDXGISwapChain> swap_chain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> render_target_view;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depth_stencil_view;

	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler_state;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depth_stencil_state;
	Microsoft::WRL::ComPtr<ID3D11BlendState> blend_state;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizer_state;

	Microsoft::WRL::ComPtr<ID3D11VertexShader> sprite_vertex_shader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> sprite_input_layout;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> sprite_pixel_shader;

	std::shared_ptr<sprite> background_sprite;
	std::shared_ptr<sprite> cloud_sprite;

	std::unique_ptr<sprite> font_sprite;
	std::unique_ptr<sprite> font_sprite_d;
	std::unique_ptr<sprite> foreground_sprite;
	std::unique_ptr<sprite> fade_sprite;

	FlameFont flameFont;
	DissolveShader dissolveShader;
	CloudShader cloudShader;
	Player player;
	Enemy enemy;
	Boss boss;

	std::vector<Shot> listOfShots{512}; 
	std::vector<Shot> listOfEnemyShots{ 512 };
	std::vector<Enemy> listOfEnemies{100};

	float timer;
	float oldTimer;
	float waveStarted;
	bool checkKey = false;
	bool checkWave = false;
	int sceneNumber = 0;
	int waveNumber = 0;
	int waveAnimation = 0;

	framework(HWND hwnd);
	~framework();

	framework(const framework&) = delete;
	framework& operator=(const framework&) = delete;
	framework(framework&&) noexcept = delete;
	framework& operator=(framework&&) noexcept = delete;

	int run()
	{
		MSG msg{};

		if (!initialize())
		{
			return 0;
		}

#ifdef USE_IMGUI
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::GetIO().Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\consola.ttf", 14.0f, nullptr, glyphRangesJapanese);
		ImGui_ImplWin32_Init(hwnd);
		ImGui_ImplDX11_Init(device.Get(), immediate_context.Get());
		ImGui::StyleColorsDark();
#endif

		while (WM_QUIT != msg.message)
		{
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{
				tictoc.tick();
				calculate_frame_stats();
				update(tictoc.time_interval());
				switch (sceneNumber)
				{
				case 0:
					renderTitle(tictoc.time_interval());
					break;
				case 1:
					renderEnemyStage(tictoc.time_interval());
					break;
				case 2:
					renderBossStage(tictoc.time_interval());
					break;
				case 10:
					renderGameOver(tictoc.time_interval());
					break;
				default:
					break;
				}
			}
		}

#ifdef USE_IMGUI
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
#endif

#if 0
		BOOL fullscreen = 0;
		swap_chain->GetFullscreenState(&fullscreen, 0);
		if (fullscreen)
		{
			swap_chain->SetFullscreenState(FALSE, 0);
		}
#endif

		return uninitialize() ? static_cast<int>(msg.wParam) : 0;
	}

	LRESULT CALLBACK handle_message(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
	{
#ifdef USE_IMGUI
		if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) { return true; }
#endif
		switch (msg)
		{
		case WM_PAINT:
		{
			PAINTSTRUCT ps{};
			BeginPaint(hwnd, &ps);

			EndPaint(hwnd, &ps);
		}
		break;

		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		case WM_CREATE:
			break;
		case WM_KEYDOWN:
			if (wparam == VK_ESCAPE)
			{
				PostMessage(hwnd, WM_CLOSE, 0, 0);
			}
			break;
		case WM_ENTERSIZEMOVE:
			tictoc.stop();
			break;
		case WM_EXITSIZEMOVE:
			tictoc.start();
			break;
		default:
			return DefWindowProc(hwnd, msg, wparam, lparam);
		}
		return 0;
	}

private:
	bool initialize();
	void update(float elapsed_time);
	void renderTitle(float elapsed_time);
	void renderEnemyStage(float elapsed_time);
	void renderBossStage(float elapsed_time);
	void renderGameOver(float elapsed_time);
	void updateEnemies(int start, int end);
	void checkEnemies(int start, int end);
	bool uninitialize();

private:
	high_resolution_timer tictoc;
	uint32_t frames{ 0 };
	float elapsed_time{ 0.0f };
	void calculate_frame_stats()
	{
		if (++frames, (tictoc.time_stamp() - elapsed_time) >= 1.0f)
		{
			float fps = static_cast<float>(frames);
			std::wostringstream outs;
			outs.precision(6);
			outs << APPLICATION_NAME << L" : FPS : " << fps << L" / " << L"Frame Time : " << 1000.0f / fps << L" (ms)";
			SetWindowTextW(hwnd, outs.str().c_str());

			frames = 0;
			elapsed_time += 1.0f;
		}
	}
};

