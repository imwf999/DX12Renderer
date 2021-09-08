#include <Windows.h>
#include <d3d12.h>
#include <wrl.h>
#include <dxgi1_4.h>
#include <iostream>
#include <tchar.h>
#include "Game.h"
#include "ConstValue.h"

#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
{
//#if defined(DEBUG) | defined(_DEBUG)
//	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
//#endif

	rdr::Game::Instance()->Initialize(hInstance);
	rdr::Game::Instance()->RunLoop();

	return 0;
}
