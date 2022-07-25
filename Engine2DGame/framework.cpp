#include "framework.h"

framework::framework(HWND hwnd) : hwnd(hwnd)
{
}

bool framework::initialize()
{
	HRESULT hr{ S_OK };

	{
		UINT create_device_flags{ 0 };
#ifdef _DEBUG
		create_device_flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

		D3D_FEATURE_LEVEL feature_levels{ D3D_FEATURE_LEVEL_11_0 };

		DXGI_SWAP_CHAIN_DESC swap_chain_desc{};
		swap_chain_desc.BufferCount = 1;
		swap_chain_desc.BufferDesc.Width = SCREEN_WIDTH;
		swap_chain_desc.BufferDesc.Height = SCREEN_HEIGHT;
		swap_chain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swap_chain_desc.BufferDesc.RefreshRate.Numerator = 60;
		swap_chain_desc.BufferDesc.RefreshRate.Denominator = 1;
		swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swap_chain_desc.OutputWindow = hwnd;
		swap_chain_desc.SampleDesc.Count = 1;
		swap_chain_desc.SampleDesc.Quality = 0;
		swap_chain_desc.Windowed = !FULLSCREEN;
		hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, create_device_flags,
			&feature_levels, 1, D3D11_SDK_VERSION, &swap_chain_desc,
			swap_chain.GetAddressOf(), device.GetAddressOf(), NULL, immediate_context.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

	{
		Microsoft::WRL::ComPtr<ID3D11Texture2D> back_buffer{};
		hr = swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<LPVOID*>(back_buffer.GetAddressOf()));
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		hr = device->CreateRenderTargetView(back_buffer.Get(), NULL, render_target_view.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

	{
		Microsoft::WRL::ComPtr<ID3D11Texture2D> depth_stencil_buffer{};
		D3D11_TEXTURE2D_DESC texture2d_desc{};
		texture2d_desc.Width = SCREEN_WIDTH;
		texture2d_desc.Height = SCREEN_HEIGHT;
		texture2d_desc.MipLevels = 1;
		texture2d_desc.ArraySize = 1;
		texture2d_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		texture2d_desc.SampleDesc.Count = 1;
		texture2d_desc.SampleDesc.Quality = 0;
		texture2d_desc.Usage = D3D11_USAGE_DEFAULT;
		texture2d_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		texture2d_desc.CPUAccessFlags = 0;
		texture2d_desc.MiscFlags = 0;
		hr = device->CreateTexture2D(&texture2d_desc, NULL, depth_stencil_buffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		D3D11_DEPTH_STENCIL_VIEW_DESC depth_stencil_view_desc{};
		depth_stencil_view_desc.Format = texture2d_desc.Format;
		depth_stencil_view_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depth_stencil_view_desc.Texture2D.MipSlice = 0;
		hr = device->CreateDepthStencilView(depth_stencil_buffer.Get(), &depth_stencil_view_desc, depth_stencil_view.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

	{
		D3D11_SAMPLER_DESC sampler_desc{};
		sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		sampler_desc.MipLODBias = 0;
		sampler_desc.MaxAnisotropy = 16;
		sampler_desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		sampler_desc.BorderColor[0] = 0;
		sampler_desc.BorderColor[1] = 0;
		sampler_desc.BorderColor[2] = 0;
		sampler_desc.BorderColor[3] = 0;
		sampler_desc.MinLOD = 0;
		sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;
		hr = device->CreateSamplerState(&sampler_desc, sampler_state.GetAddressOf());
	}

	{
		D3D11_DEPTH_STENCIL_DESC depth_stencil_desc{};
		depth_stencil_desc.DepthEnable = TRUE;
		depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		hr = device->CreateDepthStencilState(&depth_stencil_desc, depth_stencil_state.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

	{
		D3D11_BLEND_DESC blend_desc{};
		blend_desc.AlphaToCoverageEnable = FALSE;
		blend_desc.IndependentBlendEnable = FALSE;
		blend_desc.RenderTarget[0].BlendEnable = TRUE;
		blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blend_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		blend_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
		blend_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		hr = device->CreateBlendState(&blend_desc, blend_state.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

	{
		D3D11_RASTERIZER_DESC rasterizer_desc{};
		rasterizer_desc.FillMode = D3D11_FILL_SOLID;
		rasterizer_desc.CullMode = D3D11_CULL_BACK;
		rasterizer_desc.FrontCounterClockwise = FALSE;
		rasterizer_desc.DepthBias = 0;
		rasterizer_desc.DepthBiasClamp = 0;
		rasterizer_desc.SlopeScaledDepthBias = 0;
		rasterizer_desc.DepthClipEnable = TRUE;
		rasterizer_desc.ScissorEnable = FALSE;
		rasterizer_desc.MultisampleEnable = FALSE;
		rasterizer_desc.AntialiasedLineEnable = FALSE;
		hr = device->CreateRasterizerState(&rasterizer_desc, rasterizer_state.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

	{
		D3D11_BUFFER_DESC buffer_desc{};
		buffer_desc.Usage = D3D11_USAGE_DEFAULT;
		buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		buffer_desc.CPUAccessFlags = 0;
		buffer_desc.MiscFlags = 0;
		buffer_desc.StructureByteStride = 0;

	}

	{
		D3D11_INPUT_ELEMENT_DESC input_element_desc[]
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		create_vs_from_cso(device.Get(),
			"sprite_vs.cso",
			sprite_vertex_shader.GetAddressOf(),
			sprite_input_layout.GetAddressOf(),
			input_element_desc,
			ARRAYSIZE(input_element_desc));
		create_ps_from_cso(device.Get(),
			"sprite_ps.cso",
			sprite_pixel_shader.GetAddressOf());
	}

	background_sprite = std::make_shared<sprite>(device.Get(), L".\\resources\\background.png");
	cloud_sprite = std::make_shared<sprite>(device.Get(), L".\\resources\\background.png");
	fade_sprite = std::make_unique<sprite>(device.Get(), L".\\resources\\fade.png");
	//foreground_sprite = std::make_unique<sprite>(device.Get(), L".\\resources\\ammo\\bossAttackAnim\\boss_attack.png");
	foreground_sprite = std::make_unique<sprite>(device.Get(), L".\\resources\\center.png");

	font_sprite = std::make_unique<sprite>(device.Get(), L".\\resources\\fonts\\font4.png");
	font_sprite_d = std::make_unique<sprite>(device.Get(), L".\\resources\\fonts\\font_orig.png");

	flameFont.init(device.Get());
	dissolveShader.init(device.Get(), background_sprite);
	player.init(device.Get(), listOfShots);
	cloudShader.init(device.Get(), cloud_sprite);
	enemy.init(device.Get(), &player);

	for (auto p = listOfShots.begin(); p!= listOfShots.end(); ++p)
	{
		p->init(device.Get());
	}

	for (auto p = listOfEnemyShots.begin(); p != listOfEnemyShots.end(); ++p)
	{
		p->initEnemy(device.Get());
	}

	for (auto p = listOfEnemies.begin(); p != listOfEnemies.end(); ++p)
	{
		p->init(device.Get(), &player);
		auto index = std::distance(listOfEnemies.begin(), p);
		p->setState(index);
		p->update(listOfEnemyShots);
	}
	srand(time(0));
	return true;
}

void framework::update(float elapsed_time/*Elapsed seconds from last frame*/)
{
#ifdef USE_IMGUI
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
#endif


#ifdef USE_IMGUI
	ImGui::Begin("ImGUI");
	ImGui::SliderFloat("TIMER", &timer, 0.0f, +3600.0f);

	ImGui::End();
#endif

	timer += elapsed_time;

	if (player.getAct() != 0)
	{
		player.setAct(1);

		if (GetAsyncKeyState(0x57) < 0)
		{
			player.update({ 5 * sinf(player.getAngle() * PI / 180.0f), 5 * -cosf(player.getAngle() * PI / 180.0f) }, 0, 0);
			player.setAct(2);
		}
		if (GetAsyncKeyState(0x53) < 0)
		{
			player.update({ -3 * sinf(player.getAngle() * PI / 180.0f), -3 * -cosf(player.getAngle() * PI / 180.0f) }, 0, 0);
			player.setAct(3);
		}

		if (GetAsyncKeyState(0x41) < 0)
		{
			player.update({ 0, 0 }, -3, 0);
		}

		if (GetAsyncKeyState(0x44) < 0)
		{
			player.update({ 0, 0 }, 3, 0);
		}

		if (GetAsyncKeyState(0x20) < 0)
		{
			if (player.getState() == -1)
			{
				player.update({ 0, 0 }, 0, 3);
			}
			else
			{
				player.update({ 0, 0 }, 0, 1);
			}
		}

		if (GetAsyncKeyState(0x51) < 0)
		{
			player.update({ 0, 0 }, 0, 9);
		}

		player.update({ 0,0 }, 0, 0);
	}

	for (auto p = listOfShots.begin(); p != listOfShots.end(); ++p)
	{
		if (p->getPos().x < - 256 || p->getPos().x > 1280 + 256 || p->getPos().y < -256 || p->getPos().y > 1280 + 256)
		{
			p->setAct(10);
		}
		if (p->getAct() != 10)
		{
			p->update(player.getPos(), player.getAngle(), 0);
		}
	}

	for (auto p = listOfEnemyShots.begin(); p != listOfEnemyShots.end(); ++p)
	{
		if (p->getPos().x < -256 || p->getPos().x > 1280 + 256 || p->getPos().y < -256 || p->getPos().y > 1280 + 256)
		{
			p->setAct(10);
		}
		if (p->getAct() != 10)
		{
			p->updateEnemy(enemy.getPos(), enemy.getAngle());
		}
	}

	switch (waveNumber)
	{
		case 1:
			updateEnemies(0, 1);
			checkEnemies(0, 1);
			if (waveAnimation != 1)
			{
				waveStarted = timer;
				waveAnimation = 1;
			}
			break;
		case 2:
			updateEnemies(1, 3);
			checkEnemies(1, 3);
			if (waveAnimation != 2)
			{
				waveStarted = timer;
				waveAnimation = 2;
			}
			break;
		case 3:
			updateEnemies(3, 5);
			checkEnemies(3, 5);
			if (waveAnimation != 3)
			{
				waveStarted = timer;
				waveAnimation = 3;
			}
			break;
		case 4:
			updateEnemies(5, 8);
			checkEnemies(5, 8);
			if (waveAnimation != 4)
			{
				waveStarted = timer;
				waveAnimation = 4;
			}
			break;
		case 5:
			updateEnemies(8, 13);
			checkEnemies(8, 13);
			if (waveAnimation != 5)
			{
				waveStarted = timer;
				waveAnimation = 5;
			}
			break;
		case 6:
			updateEnemies(13, 21);
			checkEnemies(13, 21);
			if (waveAnimation != 6)
			{
				waveStarted = timer;
				waveAnimation = 6;
			}
			break;
		case 7:
			updateEnemies(21, 34);
			checkEnemies(21, 34);
			if (waveAnimation != 7)
			{
				waveStarted = timer;
				waveAnimation = 7;
			}
			break;
	}

	if (timer - oldTimer > 0.1f)
	{
		checkKey = false;
	}

	if (GetAsyncKeyState(VK_RETURN) < 0)
	{
		sceneNumber = 1;
		waveNumber = 1;
	}

	if (GetAsyncKeyState(0x31) < 0)
	{
		sceneNumber = 0;
	}

	if (GetAsyncKeyState(0x32) < 0)
	{
		sceneNumber = 1;
	}

	if (GetAsyncKeyState(0x33) < 0)
	{
		sceneNumber = 2;
	}

	if (GetAsyncKeyState(0x39) < 0)
	{
		sceneNumber = 10;
	}

	if (waveNumber == 8)
	{
		sceneNumber++;
		waveNumber++;
	}

	if (player.getHP() == 0)
	{
		sceneNumber = 10;
	}
	
	judge(listOfShots, listOfEnemies);
	judgePlayer(player, listOfEnemyShots);
}
void framework::renderTitle(float elapsed_time/*Elapsed seconds from last frame*/)
{

	FLOAT color[]{ 0.2f, 0.2f, 0.2f, 1.0f };
	immediate_context->ClearRenderTargetView(render_target_view.Get(), color);
	immediate_context->ClearDepthStencilView(depth_stencil_view.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	immediate_context->OMSetRenderTargets(1, render_target_view.GetAddressOf(), depth_stencil_view.Get());

	D3D11_VIEWPORT viewport{};
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = static_cast<float>(SCREEN_WIDTH);
	viewport.Height = static_cast<float>(SCREEN_HEIGHT);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	immediate_context->RSSetViewports(1, &viewport);
	immediate_context->OMSetBlendState(blend_state.Get(), nullptr, 0xFFFFFFFF);
	immediate_context->OMSetDepthStencilState(depth_stencil_state.Get(), 0);
	immediate_context->RSSetState(rasterizer_state.Get());

	background_sprite->render(immediate_context.Get(), 0, 0, 1280, 720);


	if (font_sprite)
	{
		immediate_context->IASetInputLayout(sprite_input_layout.Get());
		immediate_context->VSSetShader(sprite_vertex_shader.Get(), nullptr, 0);
		immediate_context->PSSetShader(sprite_pixel_shader.Get(), nullptr, 0);
		immediate_context->PSSetSamplers(0, 1, sampler_state.GetAddressOf());

		font_sprite->textout(immediate_context.Get(), "Development", 0, 0, 50, 40, 1.0f, 0.0f, 1.0f, 1.0f);

		if (((int)(timer)) % 2 == 0)
		{
			font_sprite_d->textout2(immediate_context.Get(), "START", 300, 400, 150, 120, 1.0f, 0.0f, 0.0f, 1.0f);
		}
		
	}

	flameFont.render(device.Get(), immediate_context.Get(), timer);

#ifdef USE_IMGUI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
#endif
 
	UINT sync_interval{ 1 };
	swap_chain->Present(sync_interval, 0);

}

void framework::renderEnemyStage(float elapsed_time/*Elapsed seconds from last frame*/)
{

	D3D11_VIEWPORT viewport{};
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = static_cast<float>(SCREEN_WIDTH);
	viewport.Height = static_cast<float>(SCREEN_HEIGHT);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	immediate_context->RSSetViewports(1, &viewport);
	immediate_context->OMSetBlendState(blend_state.Get(), nullptr, 0xFFFFFFFF);
	immediate_context->OMSetDepthStencilState(depth_stencil_state.Get(), 0);
	immediate_context->RSSetState(rasterizer_state.Get());

	cloudShader.render(device.Get(), immediate_context.Get(), timer);

	player.render(device.Get(), immediate_context.Get(), timer);

	//foreground_sprite->render(immediate_context.Get(), player.getPos().x + 60, player.getPos().y + 60, 8, 8);

	for (auto p = listOfShots.begin(); p != listOfShots.end(); ++p)
	{
		if (p->getAct() != 10)
		{
			p->render(device.Get(), immediate_context.Get(), timer);
		}
	}

	for (auto p = listOfEnemyShots.begin(); p != listOfEnemyShots.end(); ++p)
	{
		if (p->getAct() != 10)
		{
			p->renderEnemy(device.Get(), immediate_context.Get(), timer);
		}
	}

	if (timer - waveStarted <= 3)
	{
		switch (waveNumber)
		{
			case 1:
				font_sprite->textout(immediate_context.Get(), "WAVE 1", 180, 250, 150, 120, 1.0f, 0.0f, 1.0f, 1.0f);
				break;
			case 2:
				font_sprite->textout(immediate_context.Get(), "WAVE 2", 180, 250, 150, 120, 1.0f, 0.0f, 1.0f, 1.0f);
				break;
			case 3:
				font_sprite->textout(immediate_context.Get(), "WAVE 3", 180, 250, 150, 120, 1.0f, 0.0f, 1.0f, 1.0f);
				break;
			case 4:
				font_sprite->textout(immediate_context.Get(), "WAVE 4", 180, 250, 150, 120, 1.0f, 0.0f, 1.0f, 1.0f);
				break;
			case 5:
				font_sprite->textout(immediate_context.Get(), "WAVE 5", 180, 250, 150, 120, 1.0f, 0.0f, 1.0f, 1.0f);
				break;
			case 6:
				font_sprite->textout(immediate_context.Get(), "WAVE 6", 180, 250, 150, 120, 1.0f, 0.0f, 1.0f, 1.0f);
				break;
			case 7:
				font_sprite->textout(immediate_context.Get(), "WAVE 7", 180, 250, 150, 120, 1.0f, 0.0f, 1.0f, 1.0f);
				break;
			default:
				break;

		}
	}

	for (auto p = listOfEnemies.begin(); p != listOfEnemies.end(); ++p)
	{
		p->render(device.Get(), immediate_context.Get(), timer);
		//foreground_sprite->render(immediate_context.Get(), p->getPos().x + 28, p->getPos().y + 30, 8, 8);
	}

	dissolveShader.render(device.Get(), immediate_context.Get());


#ifdef USE_IMGUI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
#endif

	UINT sync_interval{ 1 };
	swap_chain->Present(sync_interval, 0);

}

void framework::renderBossStage(float elapsed_time/*Elapsed seconds from last frame*/)
{

	FLOAT color[]{ 0.2f, 0.2f, 0.2f, 1.0f };
	immediate_context->ClearRenderTargetView(render_target_view.Get(), color);
	immediate_context->ClearDepthStencilView(depth_stencil_view.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	immediate_context->OMSetRenderTargets(1, render_target_view.GetAddressOf(), depth_stencil_view.Get());

	D3D11_VIEWPORT viewport{};
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = static_cast<float>(SCREEN_WIDTH);
	viewport.Height = static_cast<float>(SCREEN_HEIGHT);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	immediate_context->RSSetViewports(1, &viewport);
	immediate_context->OMSetBlendState(blend_state.Get(), nullptr, 0xFFFFFFFF);
	immediate_context->OMSetDepthStencilState(depth_stencil_state.Get(), 0);
	immediate_context->RSSetState(rasterizer_state.Get());

	cloudShader.render(device.Get(), immediate_context.Get(), timer);

	player.render(device.Get(), immediate_context.Get(), timer);

#ifdef USE_IMGUI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
#endif

	UINT sync_interval{ 1 };
	swap_chain->Present(sync_interval, 0);

}

void framework::renderGameOver(float elapsed_time/*Elapsed seconds from last frame*/)
{

	FLOAT color[]{ 0.2f, 0.2f, 0.2f, 1.0f };
	immediate_context->ClearRenderTargetView(render_target_view.Get(), color);
	immediate_context->ClearDepthStencilView(depth_stencil_view.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	immediate_context->OMSetRenderTargets(1, render_target_view.GetAddressOf(), depth_stencil_view.Get());

	D3D11_VIEWPORT viewport{};
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = static_cast<float>(SCREEN_WIDTH);
	viewport.Height = static_cast<float>(SCREEN_HEIGHT);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	immediate_context->RSSetViewports(1, &viewport);
	immediate_context->OMSetBlendState(blend_state.Get(), nullptr, 0xFFFFFFFF);
	immediate_context->OMSetDepthStencilState(depth_stencil_state.Get(), 0);
	immediate_context->RSSetState(rasterizer_state.Get());

	dissolveShader.setTime(0);
	dissolveShader.render(device.Get(), immediate_context.Get());

	fade_sprite->render(immediate_context.Get(), 0, 0, 1280, 720);

	font_sprite->textout(immediate_context.Get(), "GAME OVER", 0, 250, 142, 120, 1.0f, 1.0f, 1.0f, 1.0f);

#ifdef USE_IMGUI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
#endif

	UINT sync_interval{ 1 };
	swap_chain->Present(sync_interval, 0);

}

void framework::updateEnemies(int start, int end)
{
	for (auto p = listOfEnemies.begin() + start; p != listOfEnemies.begin() + end; ++p)
	{
		p->update(listOfEnemyShots);
	}
}

void framework::checkEnemies(int start, int end)
{
	for (auto p = listOfEnemies.begin() + start; p != listOfEnemies.begin() + end; ++p)
	{
		if (p->getAct() != 0)
		{
			return;
		}
	}
	waveNumber++;
}

bool framework::uninitialize()
{
	return true;
}

framework::~framework()
{

}