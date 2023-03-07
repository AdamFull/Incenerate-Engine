#include "ParticlesCore.h"

using namespace engine::particles;

void CParticlesCore::create()
{
	pParticleObjectManager = std::make_unique<CObjectManager<CParticleObject>>();

	pRenderer = std::make_unique<CParticleRenderer>();

	//efkManager = Effekseer::Manager::Create(8000);

	//pRenderer->create(efkManager);
}

void CParticlesCore::update(float fDT)
{
	fTime += fDT;

	pParticleObjectManager->performDeletion();

	//Effekseer::Manager::UpdateParameter updateParameter;
	//efkManager->Update(updateParameter);

	pRenderer->setTime(fTime);

	pRenderer->begin();

	//auto efkRenderer = pRenderer->getEffekseerRenderer();

	//Effekseer::Manager::DrawParameter drawParameter;
	//drawParameter.ZNear = fNear;
	//drawParameter.ZFar = fFar;
	//drawParameter.ViewProjectionMatrix = efkRenderer->GetCameraProjectionMatrix();
	//efkManager->Draw(drawParameter);

	pRenderer->end();
}

void CParticlesCore::update_camera(float zNear, float zFar, const glm::mat4& view, const glm::mat4& projection)
{
	//auto efkRenderer = pRenderer->getEffekseerRenderer();

	//fNear = zNear;
	//fFar = zFar;
	//
	//Effekseer::Matrix44 efkView;
	//for (int x = 0; x < 4; x++)
	//	for (int y = 0; y < 4; y++)
	//		efkView.Values[y][x] = view[y][x];
	//
	//Effekseer::Matrix44 efkProjection;
	//for (int x = 0; x < 4; x++)
	//	for (int y = 0; y < 4; y++)
	//		efkProjection.Values[y][x] = projection[y][x];

	//efkRenderer->SetProjectionMatrix(efkProjection);
	//efkRenderer->SetCameraMatrix(efkView);
}

size_t CParticlesCore::addParticle(const std::string& name)
{
	//return pParticleObjectManager->add(name, std::make_unique<CParticleObject>(efkManager));
	return pParticleObjectManager->add(name, std::make_unique<CParticleObject>());
}

size_t CParticlesCore::addParticle(const std::string& name, std::unique_ptr<CParticleObject>&& source)
{
	return pParticleObjectManager->add(name, std::move(source));
}

void CParticlesCore::removeParticle(const std::string& name)
{
	pParticleObjectManager->remove(name);
}

void CParticlesCore::removeParticle(size_t id)
{
	pParticleObjectManager->remove(id);
}

const std::unique_ptr<CParticleObject>& CParticlesCore::getParticle(const std::string& name)
{
	return pParticleObjectManager->get(name);
}

const std::unique_ptr<CParticleObject>& CParticlesCore::getParticle(size_t id)
{
	return pParticleObjectManager->get(id);
}