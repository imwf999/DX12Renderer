#pragma once
#include "ConstValue.h"
#include <d3d12.h>
#include <wrl.h>
#include <memory>
#include <dxgi1_6.h>
#include "d3dx12.h"

namespace rdr
{
	using Microsoft::WRL::ComPtr;
	class PipelineStateObject;
	class Display;
	class Commond;
	class DescriptorPool;
	class Mesh;
	class TexturePool;
	class HeapManager;
	class Camera;
	class ShaderPool;
	class MaterialPool;
	class Shadow;
	class SSAOMap;
	class GBuffer;
	class FrameConstBuffer;

	class Renderer
	{
	public:
		Renderer();
		~Renderer();

	public:
		void Draw();
		void Update(const Camera& camera);
		Display* GetDisplay() const { return pDisplay.get(); }
		Commond* GetCommond() const { return pCommond.get(); }
		DescriptorPool* GetDescPool() const { return pDescriptorPool.get(); }
		HeapManager* GetResMgr() const { return pHeapResMgr.get(); }
		TexturePool* GetTexPool() const { return pTexturePool.get(); }
		ID3D12Device4* GetDevice() const { return pd3dDevice.Get(); }
		ShaderPool* GetShaderPool() const { return pShaderPool.get(); }
		MaterialPool* GetMaterialPool() const { return pMaterialPool.get(); }
		FrameConstBuffer* GetFrameCBuffer() const { return ptrFrameCBuffer.get(); }
		const std::vector<std::unique_ptr<Mesh>>& GetMeshVec() const { return meshVec; }

	private:
		void DrawMesh(ID3D12GraphicsCommandList* pList);
		void CreateDevice();
		void LoadMesh();

	private:
		std::unique_ptr<Display> pDisplay;
		std::unique_ptr<Commond> pCommond;
		std::unique_ptr<DescriptorPool> pDescriptorPool;
		std::unique_ptr<HeapManager> pHeapResMgr;
		std::unique_ptr<TexturePool> pTexturePool;
		std::unique_ptr<ShaderPool> pShaderPool;
		std::unique_ptr<MaterialPool> pMaterialPool;
		std::unique_ptr<SSAOMap> ptrSSAOMap;
		std::unique_ptr<Shadow> ptrShadowMap;
		std::unique_ptr<GBuffer> ptrGBuffer;
		std::unique_ptr<FrameConstBuffer> ptrFrameCBuffer;
		std::vector<std::unique_ptr<Mesh>> meshVec;
		uint32_t MainDSVIndex;
		const PipelineStateObject* ptrSkyBoxPso;

		//TODO:待修改
	private:
		ComPtr<IDXGIFactory5> pIDXGIFactory5;
		ComPtr<ID3D12Device4> pd3dDevice;
		ComPtr<IDXGIAdapter1> pAdapter;
	};

}
