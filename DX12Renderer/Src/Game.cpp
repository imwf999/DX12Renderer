#include "Game.h"
#include "ConstValue.h"
#include "Renderer.h"
#include "GameTimer.h"
#include "Input.h"
#include "Camera.h"
#include <windowsx.h>
#include "Audio.h"
#include "UIManager.h"

namespace rdr
{
	LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		return Game::Instance()->ScreenEvent(hwnd, msg, wParam, lParam);
	}

	Game::Game() 
		: appInstance(nullptr), 
		mainWinHandle(nullptr),
		pRenderer(nullptr),
		winName(L"DX12Renderer")
	{
		
	}

	Game* Game::Instance()
	{
		static Game game;
		return &game;
	}

	void Game::InitMainWindow()
	{
		WNDCLASS wc;
		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc = WndProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = appInstance;
		wc.hIcon = LoadIcon(0, IDI_APPLICATION);
		wc.hCursor = LoadCursor(0, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
		wc.lpszMenuName = 0;
		wc.lpszClassName = L"MainWnd";

		RegisterClass(&wc);

		this->mainWinHandle = CreateWindow(
			L"MainWnd",
			winName.c_str(),
			WS_OVERLAPPEDWINDOW,
			global_WindowLeft,
			global_WindowTop,
			global_WindowWidth,
			global_WindowHeight,
			NULL,
			NULL,
			this->appInstance,
			NULL
		);

		ShowWindow(mainWinHandle, SW_SHOW);
		UpdateWindow(mainWinHandle);
	}

	void Game::Initialize(HINSTANCE appIns)
	{
		appInstance = appIns;
		InitMainWindow();
		pGameTimer = std::make_unique<GameTimer>();
		pCamera = std::make_shared<Camera>();
		ptrAudio = std::make_unique<Audio>();
		pRenderer = std::make_unique<Renderer>();
		ptrUIManager = std::make_unique<UIManager>();
		ptrUIManager->Initialize(*pRenderer->GetUIRenderer());
		pInput = std::make_unique<Input>([&](float dx, float dy) {pCamera->RotateX(dy); pCamera->RotateY(dx); });
	}

	LRESULT Game::ScreenEvent(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		switch (msg)
		{
		case MK_SHIFT:
			pCamera->SetMoveSpeed(400);
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		case WM_RBUTTONDOWN:
			pInput->OnRightMouseDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			break;
		case WM_LBUTTONDOWN:
			pInput->OnLeftMouseDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			break;
		case WM_RBUTTONUP:
			pInput->OnRightMouseUp();
			break;
		case WM_MOUSEMOVE:
			pInput->OnMouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			break;
		default:
			return DefWindowProc(hwnd, msg, wParam, lParam);
		}
		return 0;
	}

	void Game::RunLoop()
	{
		MSG msg = { 0 };

		pGameTimer->Reset();

		while (msg.message != WM_QUIT)
		{
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{
				pGameTimer->Tick();
				pInput->Update(*pGameTimer, *pCamera, *pRenderer);
				pCamera->Update();
				pRenderer->Update(*pCamera);
				ptrUIManager->Update();
				pRenderer->Draw();
			}
		}
	}
}
