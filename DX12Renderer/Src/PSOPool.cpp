#include "PSOPool.h"
#include "DefaultPSO.h"
#include "SkyboxPSO.h"
#include "ShadowMapPSO.h"
#include "RendererFacade.h"
#include "PassPool.h"
#include "Display.h"
namespace rdr
{
	PSOPool::PSOPool(const RendererFacade& renderer)
	{
		std::unique_ptr<IPipelineState> defaulPSO = std::make_unique<DefaultPSO>();
		defaulPSO->Create((*renderer.GetPassPool())[0], renderer.GetDevice());
		std::unique_ptr<IPipelineState> shadowPSO = std::make_unique<ShadowMapPSO>();
		shadowPSO->Create((*renderer.GetPassPool())[1], renderer.GetDevice());
		std::unique_ptr<IPipelineState> skyboxPSO = std::make_unique<SkyboxPSO>();
		skyboxPSO->Create((*renderer.GetPassPool())[2], renderer.GetDevice());
		psoVec.emplace_back(std::move(defaulPSO));
		psoVec.emplace_back(std::move(shadowPSO));
		psoVec.emplace_back(std::move(skyboxPSO));
	}

	PSOPool::~PSOPool()
	{
	}
}