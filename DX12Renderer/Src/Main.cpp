#include <Windows.h>
#include <d3d12.h>
#include <wrl.h>
#include <dxgi1_4.h>
#include <iostream>
#include <tchar.h>
#include "Game.h"
#include "ConstValue.h"
#include "RendererHelper.h"

//#if defined(DEBUG) || defined(_DEBUG)
//#define _CRTDBG_MAP_ALLOC
//#include <crtdbg.h>
//#endif

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
{
	try
	{
		rdr::Game::Instance()->Initialize(hInstance);
		rdr::Game::Instance()->RunLoop();
	}
	catch (const rdr::DxException& e)
	{
		MessageBeep(-1);
		MessageBox(rdr::Game::Instance()->GetMainWinHandle(), e.ToString().c_str(), L"Error", MB_OK);
		exit(-1);
	}
	return 0;
}
