#include "RootSignature.h"
#include "RendererHelper.h"

namespace rdr
{
	RootSignature::~RootSignature() = default;

	void RootSignature::Create(ID3D12Device* pDevice, const std::function<void()>& deletePtrFunc)
	{
		//检查是否支持1.1版本的根签名，如果不支持就用1.0版本
		D3D12_FEATURE_DATA_ROOT_SIGNATURE sigFeature = {};
		sigFeature.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;
		if (FAILED(pDevice->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &sigFeature, sizeof(sigFeature))))
			sigFeature.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;

		CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSigDesc = {};
		rootSigDesc.Init_1_1(
			static_cast<UINT>(rootParameterVec.size()),
			rootParameterVec.data(),
			0,
			nullptr,
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		ComPtr<ID3DBlob> pSignatureBlob;
		ComPtr<ID3DBlob> pErrorBlob;

		HRESULT hr = D3DX12SerializeVersionedRootSignature(
			&rootSigDesc,
			sigFeature.HighestVersion,
			pSignatureBlob.GetAddressOf(),
			pErrorBlob.GetAddressOf()
		);
#if defined(_DEBUG) || defined(DEBUG)
		if (pErrorBlob != nullptr)
			OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());
#endif
		ThrowIfFailed(pDevice->CreateRootSignature(
			0,
			pSignatureBlob->GetBufferPointer(),
			pSignatureBlob->GetBufferSize(),
			IID_PPV_ARGS(pRootSignature.GetAddressOf())));

		deletePtrFunc();
	}
}
