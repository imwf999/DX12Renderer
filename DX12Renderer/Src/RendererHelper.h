#pragma once

#include <comdef.h>
#include <string>
#include <wrl.h>
#include <dxgi1_6.h>
#include <d3d12.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>

namespace rdr
{
	using Microsoft::WRL::ComPtr;

	class DxException
	{
	public:
		DxException() = default;
		DxException(HRESULT hr, const std::wstring& functionName, const std::wstring& filename, int lineNumber)
			: ErrorCode(hr),
			FunctionName(functionName),
			Filename(filename),
			LineNumber(lineNumber)
		{

		}

		std::wstring ToString()const
		{
			_com_error err(ErrorCode);
			std::wstring msg = err.ErrorMessage();

			return FunctionName + L" failed in " + Filename + L"; line " + std::to_wstring(LineNumber) + L"; error: " + msg;
		}

		HRESULT ErrorCode = S_OK;
		std::wstring FunctionName;
		std::wstring Filename;
		int LineNumber = -1;
	};

	inline std::wstring AnsiToWString(const std::string& str)
	{
		WCHAR buffer[512];
		MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, buffer, 512);
		return std::wstring(buffer);
	}

#ifndef ThrowIfFailed
#define ThrowIfFailed(x)                                              \
{                                                                     \
    HRESULT hr__ = (x);                                               \
    std::wstring wfn = AnsiToWString(__FILE__);                       \
    if(FAILED(hr__)) { throw DxException(hr__, L#x, wfn, __LINE__); } \
}
#endif

#ifndef ReleaseCom
#define ReleaseCom(x) { if(x){ x->Release(); x = 0; } }
#endif

#define UpperAlignment(A,B) ((UINT)(((A)+((B)-1))&~(B - 1)))

	inline ComPtr<ID3DBlob> CompileShader(
		const std::wstring& filename,
		const D3D_SHADER_MACRO* defines,
		const std::string& entrypoint,
		const std::string& target)
	{
		UINT compileFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)  
		compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

		HRESULT hr = S_OK;

		ComPtr<ID3DBlob> byteCode = nullptr;
		ComPtr<ID3DBlob> errors;
		hr = D3DCompileFromFile(filename.c_str(), defines, D3D_COMPILE_STANDARD_FILE_INCLUDE,
			entrypoint.c_str(), target.c_str(), compileFlags, 0, &byteCode, &errors);

		if (errors != nullptr)
			OutputDebugStringA((char*)errors->GetBufferPointer());

		ThrowIfFailed(hr);

		return byteCode;
	}

	static std::string WstrToStr(const std::wstring& wstr)
	{
		int length = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), static_cast<int>(wstr.length()), nullptr, 0, nullptr, nullptr);
		std::string tempStr(length, ' ');
		WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), static_cast<int>(wstr.length()), const_cast<char*>(tempStr.c_str()), length, nullptr, nullptr);
		return tempStr;
	}

	static DirectX::XMMATRIX InverseTranspose(DirectX::CXMMATRIX M)
	{
		DirectX::XMMATRIX A = M;
		A.r[3] = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

		DirectX::XMVECTOR det = DirectX::XMMatrixDeterminant(A);
		return DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(&det, A));
	}

	//通过模型名获取到文件夹中对应的纹理和法线名
	static std::tuple<std::string, std::string> GetTextureFromMesh(const std::string& meshName)
	{
		std::string tempName = meshName;
		for (size_t length = tempName.size(), i = 0, count = 0; i < length; ++i)
		{
			if (tempName[i] == '_') ++count;
			if (count == 4)
			{
				tempName = tempName.substr(i + 1);
				break;
			}
		}
		std::string diffuseName = tempName + "_diffuse", normalName;
		if (tempName.substr(0, 6) == "fabric") normalName = "fabric_normal";
		else normalName = tempName + "_normal";
		std::tuple<std::string, std::string> result = std::make_tuple(diffuseName, normalName);
		return result;
	}
}
