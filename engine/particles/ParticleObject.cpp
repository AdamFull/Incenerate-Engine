#include "ParticleObject.h"

#include "filesystem/vfs_helper.h"

using namespace engine::filesystem;
using namespace engine::particles;

//CParticleObject::CParticleObject(Effekseer::ManagerRef manager)
//{
//	efkManager = manager;
//}

void CParticleObject::load(const std::filesystem::path& path)
{
	//auto fullpath = fs::get_workdir() / path;
	//auto u16_path = fullpath.u16string();

	//efkEffect = Effekseer::Effect::Create(efkManager, u16_path.c_str());
}

void CParticleObject::play(const glm::vec3& position)
{
	//Effekseer::Vector3D pos{ position.x, position.y, position.z };
	//efkHandle = efkManager->Play(efkEffect, pos);
}

void CParticleObject::stop()
{
	//efkManager->StopEffect(efkHandle);
}