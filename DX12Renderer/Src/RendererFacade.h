#pragma once
#include "ConstValue.h"
#include <d3d12.h>
#include <wrl.h>
#include <memory>
#include <dxgi1_6.h>
#include "d3dx12.h"
#include "RenderObject.h"
#include "Mesh.h"

namespace rdr
{
	using Microsoft::WRL::ComPtr;
	class Display;
	class Commond;
	class PassPool;
	class DescriptorPool;
	class Mesh;
	class PSOPool;
	class TexturePool;
	class HeapResourceManager;
	class Camera;
	class ConstBufferPool;

	class RendererFacade
	{
	public:
		RendererFacade();
		~RendererFacade();

	public:
		void Draw();
		void Update(const Camera& camera);
		Display* GetDisplay() const { return pDisplay.get(); }
		Commond* GetCommond() const { return pCommond.get(); }
		PassPool* GetPassPool() const { return pPassPool.get(); }
		DescriptorPool* GetDescPool() const { return pDescriptorPool.get(); }
		PSOPool* GetPSOPool() const { return pPSOPool.get(); }
		HeapResourceManager* GetResMgr() const { return pHeapResMgr.get(); }
		TexturePool* GetTexPool() const { return pTexturePool.get(); }
		ConstBufferPool* GetCBufferPool() const { return pConstBufferPool.get(); }
		ID3D12Device4* GetDevice() const { return pd3dDevice.Get(); }

	private:
		void DrawObject(ID3D12GraphicsCommandList* pList, RenderObject& object);
		void CreateDevice();

	private:
		std::unique_ptr<Display> pDisplay;
		std::unique_ptr<Commond> pCommond;
		std::unique_ptr<PassPool> pPassPool;
		std::unique_ptr<DescriptorPool> pDescriptorPool;
		std::unique_ptr<PSOPool> pPSOPool;
		std::unique_ptr<HeapResourceManager> pHeapResMgr;
		std::unique_ptr<TexturePool> pTexturePool;
		std::unique_ptr<ConstBufferPool> pConstBufferPool;
		std::vector<RenderObject> renderObjectVec;
		std::vector<std::unique_ptr<Mesh>> meshVec;

		//TODO:待修改
	private:
		ComPtr<IDXGIFactory5> pIDXGIFactory5;
		ComPtr<ID3D12Device4> pd3dDevice;
		ComPtr<IDXGIAdapter1> pAdapter;
	};
}