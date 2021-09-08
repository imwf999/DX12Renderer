#include "PassPool.h"
#include "Pass.h"

namespace rdr
{
	PassPool::PassPool(ID3D12Device* pDevice)
	{	
		//下面这个控制shader编译成行矩阵模式
//compileFlags |= D3DCOMPILE_PACK_MATRIX_ROW_MAJOR;

		Pass::SlotInfo slot;

		std::vector<Pass::SlotInfo> slots;
		slot.type = Pass::ConstantBufferObj;
		slot.registerPosition = 0;
		slot.spacePosition = 0;
		slot.constBufferIndex = 0;
		slots.emplace_back(slot);

		slot.type = Pass::ConstantBufferPass;
		slot.registerPosition = 1;
		slot.spacePosition = 0;
		slot.constBufferIndex = 0;
		slots.emplace_back(slot);
		Pass shadowPass(slots, L"..\\Shader\\ShadowMapMain.hlsl", "VSMain", "PSMain", pDevice);

		std::vector<Pass::SlotInfo> slotVec2;
		slot.type = Pass::DescriptorHeapSRV;
		slot.registerPosition = 0;
		slot.spacePosition = 0;
		slot.tableSize = 42;
		slot.viewIndex = 0;
		slotVec2.emplace_back(slot);

		slot.type = Pass::DescriptorHeapSRV;
		slot.registerPosition = 1;
		slot.spacePosition = 2;
		slot.tableSize = 1;
		slot.viewIndex = 43;
		slotVec2.emplace_back(slot);

		slot.type = Pass::DescriptorHeapSampler;
		slot.registerPosition = 0;
		slot.spacePosition = 0;
		slot.tableSize = 2;
		slot.viewIndex = 0;
		slotVec2.emplace_back(slot);

		slot.type = Pass::ConstantBufferObj;
		slot.registerPosition = 0;
		slot.spacePosition = 0;
		slot.constBufferIndex = 0;
		slotVec2.emplace_back(slot);

		slot.type = Pass::ConstantBufferPass;
		slot.registerPosition = 1;
		slot.spacePosition = 0;
		slot.constBufferIndex = 1;
		slotVec2.emplace_back(slot);

		slot.type = Pass::ConstantBufferMat;
		slot.registerPosition = 2;
		slot.spacePosition = 0;
		slot.constBufferIndex = 0;
		slotVec2.emplace_back(slot);

#ifdef POINTSHADOW
		slot.type = Pass::DescriptorHeapSRV;
		slot.registerPosition = 2;
		slot.spacePosition = 2;
		slot.tableSize = 1;
		slot.viewIndex = 44;
		slotVec2.emplace_back(slot);
#endif // POINTSHADOW

		Pass mainPass(slotVec2, L"..\\Shader\\ShaderMain.hlsl", "VSMain", "PSMain", pDevice);

		std::vector<Pass::SlotInfo> slotVec3;
		slot.type = Pass::DescriptorHeapSRV;
		slot.registerPosition = 0;
		slot.spacePosition = 2;
		slot.tableSize = 1;
		slot.viewIndex = 42;
		slotVec3.emplace_back(slot);

		slot.type = Pass::DescriptorHeapSampler;
		slot.registerPosition = 0;
		slot.spacePosition = 0;
		slot.tableSize = 1;
		slot.viewIndex = 0;
		slotVec3.emplace_back(slot);

		slot.type = Pass::ConstantBufferObj;
		slot.registerPosition = 0;
		slot.spacePosition = 0;
		slot.constBufferIndex = 0;
		slotVec3.emplace_back(slot);

		slot.type = Pass::ConstantBufferPass;
		slot.registerPosition = 1;
		slot.spacePosition = 0;
		slot.constBufferIndex = 1;
		slotVec3.emplace_back(slot);
		Pass skyboxPass(slotVec3, L"..\\Shader\\SkyBoxMain.hlsl", "VSMain", "PSMain", pDevice);

		passVec.emplace_back(std::move(mainPass));
		passVec.emplace_back(std::move(shadowPass));
		passVec.emplace_back(std::move(skyboxPass));

#ifdef POINTSHADOW
		std::vector<Pass::SlotInfo> vecRight;
		slot.type = Pass::ConstantBufferObj;
		slot.registerPosition = 0;
		slot.spacePosition = 0;
		slot.constBufferIndex = 0;
		vecRight.emplace_back(slot);

		slot.type = Pass::ConstantBufferPass;
		slot.registerPosition = 1;
		slot.spacePosition = 0;
		slot.constBufferIndex = 2;
		vecRight.emplace_back(slot);
		Pass rightPass(vecRight, L"..\\..\\Shader\\ShadowMapMain.hlsl", "VSMain", "PSMain", pDevice);

		vecRight[1].constBufferIndex = 3;
		Pass leftPass(vecRight, L"..\\..\\Shader\\ShadowMapMain.hlsl", "VSMain", "PSMain", pDevice);

		vecRight[1].constBufferIndex = 4;
		Pass forwardPass(vecRight, L"..\\..\\Shader\\ShadowMapMain.hlsl", "VSMain", "PSMain", pDevice);

		vecRight[1].constBufferIndex = 5;
		Pass backPass(vecRight, L"..\\..\\Shader\\ShadowMapMain.hlsl", "VSMain", "PSMain", pDevice);

		vecRight[1].constBufferIndex = 6;
		Pass upPass(vecRight, L"..\\..\\Shader\\ShadowMapMain.hlsl", "VSMain", "PSMain", pDevice);

		vecRight[1].constBufferIndex = 7;
		Pass downPass(vecRight, L"..\\..\\Shader\\ShadowMapMain.hlsl", "VSMain", "PSMain", pDevice);

		passVec.emplace_back(std::move(rightPass));
		passVec.emplace_back(std::move(leftPass));
		passVec.emplace_back(std::move(forwardPass));
		passVec.emplace_back(std::move(backPass));
		passVec.emplace_back(std::move(upPass));
		passVec.emplace_back(std::move(downPass));
#endif // POINTSHADOW

	}

	PassPool::~PassPool()
	{

	}
}
