#include "ParticleRenderer.h"

#include "Engine.h"

using namespace engine::particles;

CParticleRenderer::~CParticleRenderer()
{
	//efkCommandList.Reset();
	//efkMemoryPool.Reset();
	//efkRenderer.Reset();
}

//void CParticleRenderer::create(Effekseer::ManagerRef efkManager)
//{
//	auto& graphics = EGEngine->getGraphics();
//	auto& device = graphics->getDevice();
//	auto& commandPool = device->getCommandPool();
//	
//	auto graphicsDevice = ::EffekseerRendererVulkan::CreateGraphicsDevice(device->getPhysical(),
//		device->getLogical(), device->getTransferQueue(), commandPool->getCommandPool(), device->getFramesInFlight());
//	
//	EffekseerRendererVulkan::RenderPassInformation renderPassInfo;
//	renderPassInfo.DoesPresentToScreen = true;
//	renderPassInfo.RenderTextureCount = 1;
//	renderPassInfo.RenderTextureFormats[0] = VK_FORMAT_B8G8R8A8_UNORM;
//	renderPassInfo.DepthFormat = VK_FORMAT_D24_UNORM_S8_UINT;
//	efkRenderer = EffekseerRendererVulkan::Create(graphicsDevice, renderPassInfo, 8000);
//	
//	efkMemoryPool = EffekseerRenderer::CreateSingleFrameMemoryPool(efkRenderer->GetGraphicsDevice());
//	
//	efkCommandList = EffekseerRenderer::CreateCommandList(efkRenderer->GetGraphicsDevice(), efkMemoryPool);
//	
//	efkManager->SetSpriteRenderer(efkRenderer->CreateSpriteRenderer());
//	efkManager->SetRibbonRenderer(efkRenderer->CreateRibbonRenderer());
//	efkManager->SetRingRenderer(efkRenderer->CreateRingRenderer());
//	efkManager->SetTrackRenderer(efkRenderer->CreateTrackRenderer());
//	efkManager->SetModelRenderer(efkRenderer->CreateModelRenderer());
//	
//	efkManager->SetTextureLoader(efkRenderer->CreateTextureLoader());
//	efkManager->SetModelLoader(efkRenderer->CreateModelLoader());
//	efkManager->SetMaterialLoader(efkRenderer->CreateMaterialLoader());
//	efkManager->SetCurveLoader(Effekseer::MakeRefPtr<Effekseer::CurveLoader>());
//}

void CParticleRenderer::setTime(float time)
{
	//efkRenderer->SetTime(time);
}

void CParticleRenderer::begin()
{
	auto& graphics = EGEngine->getGraphics();

	//efkMemoryPool->NewFrame();
	//
	//EffekseerRendererVulkan::BeginCommandList(efkCommandList, graphics->getCommandBuffer());
	//efkRenderer->SetCommandList(efkCommandList);
	//
	//efkRenderer->BeginRendering();
}

void CParticleRenderer::end()
{
	//efkRenderer->EndRendering();
	//
	//efkRenderer->SetCommandList(nullptr);
	//EffekseerRendererVulkan::EndCommandList(efkCommandList);
}