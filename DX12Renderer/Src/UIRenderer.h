#pragma once
#include <d3d11_4.h>
#include <d3d11on12.h>
#include <d2d1_3.h>
#include <dwrite.h>
#include <wrl.h>
#include <memory>
#include <vector>
#include "ConstValue.h"

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "d3d11.lib")

namespace rdr
{		
	using Microsoft::WRL::ComPtr;
	class Renderer;
	class Text;

	class UIRenderer
	{
	public:
		UIRenderer(const Renderer& renderer);
		~UIRenderer();

		void DrawUI(const Renderer& renderer);
		const ComPtr<ID2D1DeviceContext6>& GetDeviceContext6() const { return pID2D1DeviceContext6; }
		const ComPtr<IDWriteFactory>& GetDWriteFactory() const { return pIDWriteFactory; }

	private:
		void CreateDevice(const Renderer& renderer);
		void CreateUIRenderTarget(const Renderer& renderer);

	private:
		ComPtr<ID3D11On12Device1> pID3D11On12Device1;
		ComPtr<ID3D11Device5> pID3D11Device5;
		ComPtr<ID3D11DeviceContext4> pID3D11DeviceContext4;
		ComPtr<ID2D1Factory7> pID2D1Factory7;
		ComPtr<ID2D1Device6>	pID2D1Device6;
		ComPtr<ID2D1DeviceContext6> pID2D1DeviceContext6;
		ComPtr<IDWriteFactory> pIDWriteFactory;
		ComPtr<ID3D11Resource> pID3D11WrappedBackBuffers[global_SwapChainBufferCount];
		ComPtr<ID2D1Bitmap1> pID2DRenderTargets[global_SwapChainBufferCount];
	};
}
