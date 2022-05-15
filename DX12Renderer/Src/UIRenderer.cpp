#include "UIRenderer.h"
#include "Commond.h"
#include "Display.h"
#include "RendererHelper.h"
#include "Renderer.h"
#include <strsafe.h>
#include <tchar.h>
#include "Camera.h"
#include "Text.h"
#include "Game.h"
#include "GameTimer.h"
#include "Toggle.h"
#include "UIManager.h"

namespace rdr
{
	UIRenderer::UIRenderer(const Renderer& renderer)
	{
		CreateDevice(renderer);
		CreateUIRenderTarget(renderer);
	}

	UIRenderer::~UIRenderer()
	{
	}

	void UIRenderer::DrawUI(const Renderer& renderer)
	{
		pID3D11On12Device1->AcquireWrappedResources(pID3D11WrappedBackBuffers[renderer.GetDisplay()->GetCurrentBackBufferIndex()].GetAddressOf(), 1);
		pID2D1DeviceContext6->SetTarget(pID2DRenderTargets[renderer.GetDisplay()->GetCurrentBackBufferIndex()].Get());
		pID2D1DeviceContext6->BeginDraw();
		pID2D1DeviceContext6->SetTransform(D2D1::Matrix3x2F::Identity());
		for (const auto& item : Game::Instance()->GetUIManager()->GetUIData())
		{
			item->Draw();
		}
		pID2D1DeviceContext6->EndDraw();
		pID3D11On12Device1->ReleaseWrappedResources(pID3D11WrappedBackBuffers[renderer.GetDisplay()->GetCurrentBackBufferIndex()].GetAddressOf(), 1);
		pID3D11DeviceContext4->Flush();
	}

	void UIRenderer::CreateDevice(const Renderer& renderer)
	{
		UINT tD3D11DeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
		ComPtr<ID3D11Device> pID3D11Device;
		ComPtr<ID3D11DeviceContext> pID3D11DeviceContext;
		ComPtr<ID3D12Device> pID3D12Device;
		CHECK_RESULT(renderer.pd3dDevice.As(&pID3D12Device));
		CHECK_RESULT(D3D11On12CreateDevice(
			pID3D12Device.Get(),
			tD3D11DeviceFlags,
			nullptr,
			0,
			reinterpret_cast<IUnknown* const*>(renderer.GetCommond()->GetCmdQueueComPtr().GetAddressOf()),
			1,
			0,
			&pID3D11Device,
			&pID3D11DeviceContext,
			nullptr));
		CHECK_RESULT(pID3D11Device.As(&pID3D11Device5));
		CHECK_RESULT(pID3D11DeviceContext.As(&pID3D11DeviceContext4));
		CHECK_RESULT(pID3D11Device5.As(&pID3D11On12Device1));

		D2D1_DEVICE_CONTEXT_OPTIONS emD2DDeviceOptions = D2D1_DEVICE_CONTEXT_OPTIONS_NONE;
		CHECK_RESULT(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, IID_PPV_ARGS(&pID2D1Factory7)));
		ComPtr<IDXGIDevice> pIDXGIDevice;
		CHECK_RESULT(pID3D11On12Device1.As(&pIDXGIDevice));
		CHECK_RESULT(pID2D1Factory7->CreateDevice(pIDXGIDevice.Get(), &pID2D1Device6));
		CHECK_RESULT(pID2D1Device6->CreateDeviceContext(emD2DDeviceOptions, &pID2D1DeviceContext6));
		CHECK_RESULT(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, _uuidof(IDWriteFactory), &pIDWriteFactory));
	}

	void UIRenderer::CreateUIRenderTarget(const Renderer& renderer)
	{
		D2D1_BITMAP_PROPERTIES1 td2dProperties = D2D1::BitmapProperties1(
			D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
			D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED),
			96.0f,
			96.0f);

		for(uint32_t index = 0; index < global_SwapChainBufferCount; ++index)
		{
			D3D11_RESOURCE_FLAGS td3d11Flags = { D3D11_BIND_RENDER_TARGET };
			CHECK_RESULT(pID3D11On12Device1->CreateWrappedResource(
				renderer.GetDisplay()->GetRenderTargetArr()[index]->GetDefaultResource(),
				&td3d11Flags,
				D3D12_RESOURCE_STATE_RENDER_TARGET,
				D3D12_RESOURCE_STATE_PRESENT,
				IID_PPV_ARGS(&pID3D11WrappedBackBuffers[index])));

			ComPtr<IDXGISurface> pIDXGISurface;

			CHECK_RESULT(pID3D11WrappedBackBuffers[index].As(&pIDXGISurface));
			CHECK_RESULT(pID2D1DeviceContext6->CreateBitmapFromDxgiSurface(pIDXGISurface.Get(), &td2dProperties, &pID2DRenderTargets[index]));
		}
	}
}
