#ifndef RDR_GAME_H
#define RDR_GAME_H

#include <Windows.h>
#include <string>
#include <memory>

#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")

namespace rdr
{
	class RendererFacade;
	class GameTimer;
	class Input;
	class Camera;

	class Game
	{
	public:
		Game(const Game&) = delete;
		Game& operator= (const Game&) = delete;
		~Game() = default;

	public:
		static Game* Instance();

	public:
		HINSTANCE GetAppInstance() const { return appInstance; }
		HWND GetMainWinHandle() const { return mainWinHandle; }

	public:
		void Initialize(HINSTANCE appIns);
		void RunLoop();
		LRESULT ScreenEvent(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	private:
		Game();
		void InitMainWindow();

	private:
		HINSTANCE appInstance;		//应用程序实例句柄
		HWND mainWinHandle;				//主窗口句柄
		std::wstring winName;		//窗口名

	private:
		std::unique_ptr<RendererFacade> pRenderer;
		std::unique_ptr<GameTimer> pGameTimer;
		std::unique_ptr<Input> pInput;
		std::unique_ptr<Camera> pCamera;
	};
}

#endif
