#include "ShaderPool.h"
#include "Shader.h"
#include "ConstValue.h"
#include "PSOPool.h"
#include "RootSignature.h"
#include "PSO.h"

namespace rdr
{
	//TODO:这里链式调用可以封装成一个函数或者宏
	ShaderPool::ShaderPool(ID3D12Device4* device)
	{
		AddShader("Phong")
			.CompileVertexShader(PHONG_SHADER_FILE_PATH, "VSMain")
			.CompilePixelShader(PHONG_SHADER_FILE_PATH, "PSMain")
			.AddInputElement({ { "POSITION", 3}, {"TEXCOORD", 2}, {"NORMAL", 3}, {"TANGENT", 3 } })
			.AddSlot({ Shader::DescriptorHeapSRV, 0, 0, 1 })
			.AddSlot({ Shader::DescriptorHeapSRV, 1, 0, 1 })
			.AddSlot({ Shader::DescriptorHeapSRV, 2, 0, 1 })
#if SSAO
			.AddSlot({ Shader::DescriptorHeapSRV, 3, 0, 1 })
#endif
			.AddSlot({ Shader::DescriptorHeapSampler, 0, 0, 2 })
			.AddSlot({ Shader::ObjectCBuffer, 1, 0, 0 })
			.AddSlot({ Shader::FrameCBuffer, 0, 0, 0 })
			.CreateRootSignature(device)
			.CreatePSO(PhongPSO().desc, device);

		AddShader("ShadowMap")
			.CompileVertexShader(SHADOW_MAP_SHADER_FILE_PATH, "VSMain")
			.CompilePixelShader(SHADOW_MAP_SHADER_FILE_PATH, "PSMain")
			.AddInputElement({ { "POSITION", 3} })
			.AddSlot({ Shader::FrameCBuffer, 0, 0, 0 })
			.AddSlot({ Shader::ObjectCBuffer, 1, 0, 0 })
			.CreateRootSignature(device)
			.CreatePSO(ShadowMapPSO().desc, device);

		AddShader("SkyBox")
			.CompileVertexShader(SKYBOX_SHADER_FILE_PATH, "VSMain")
			.CompilePixelShader(SKYBOX_SHADER_FILE_PATH, "PSMain")
			.AddInputElement({ { "POSITION", 3 } })
			.AddSlot({ Shader::DescriptorHeapSRV, 0, 0, 1 })
			.AddSlot({ Shader::DescriptorHeapSampler, 0, 0, 1 })
			.AddSlot({ Shader::FrameCBuffer, 0, 0, 0 })
			.AddSlot({ Shader::ObjectCBuffer, 1, 0, 0 })
			.CreateRootSignature(device)
			.CreatePSO(SkyBoxPSO().desc, device);

		AddShader("DrawNormal")
			.CompileVertexShader(GBUFFER_SHADER_FILE_PATH, "VSMain")
			.CompilePixelShader(GBUFFER_SHADER_FILE_PATH, "PSMain")
			.AddInputElement({ { "POSITION", 3 }, {"TEXCOORD", 2}, {"NORMAL", 3} })		//顶点缓冲目前都是共用一个，所以这里必须把纹理坐标也带上,虽然并没有用到
			.AddSlot({ Shader::FrameCBuffer, 0, 0, 0 })
			.AddSlot({ Shader::ObjectCBuffer, 1, 0, 0 })
			.CreateRootSignature(device)
			.CreatePSO(DrawNormalPSO().desc, device);

#if SSAO
		AddShader("Ssao")
			.CompileVertexShader(SSAO_SHADER_FILE_PATH, "VSMain")
			.CompilePixelShader(SSAO_SHADER_FILE_PATH, "PSMain")
			.AddSlot({ Shader::DescriptorHeapSRV, 0, 0, 1 })
			.AddSlot({ Shader::DescriptorHeapSRV, 1, 0, 1 })
			.AddSlot({ Shader::DescriptorHeapSRV, 2, 0, 1 })
			.AddSlot({ Shader::DescriptorHeapSampler, 0, 0, 1 })
			.AddSlot({ Shader::DescriptorHeapSampler, 1, 0, 1 })
			.AddSlot({ Shader::DescriptorHeapSampler, 2, 0, 1 })
			.AddSlot({ Shader::FrameCBuffer, 0, 0, 0 })
			.AddSlot({ Shader::ObjectCBuffer, 1, 0, 0 })
			.CreateRootSignature(device)
			.CreatePSO(SsaoPSO().desc, device);

		AddShader("SsaoBlur")
			.CompileVertexShader(SSAOBLUR_SHADER_FILE_PATH, "VSMain")
			.CompilePixelShader(SSAOBLUR_SHADER_FILE_PATH, "PSMain")
			.AddSlot({ Shader::DescriptorHeapSRV, 0, 0, 1 })
			.AddSlot({ Shader::DescriptorHeapSRV, 1, 0, 1 })
			.AddSlot({ Shader::DescriptorHeapSampler, 0, 0, 1 })
			.AddSlot({ Shader::FrameCBuffer, 0, 0, 0 })
			.AddSlot({ Shader::ObjectCBuffer, 1, 0, 0 })
			.CreateRootSignature(device)
			.CreatePSO(SsaoPSO().desc, device);
#endif
	}

	ShaderPool::~ShaderPool() = default;

	Shader& ShaderPool::AddShader(const std::string& name)
	{
		std::shared_ptr<Shader> ptrShader = std::make_shared<Shader>(name);
		ShaderMap.insert({ name, ptrShader});
		return *ShaderMap[name].get();
	}

}
