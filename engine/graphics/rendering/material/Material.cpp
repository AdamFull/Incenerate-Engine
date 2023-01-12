#include "Material.h"

#include "Engine.h"

using namespace engine::graphics;

CMaterial::~CMaterial()
{
	EGGraphics->removeShader(shader_id);

	for (auto& [name, id] : mTextures)
		EGGraphics->removeImage(id);
}

void CMaterial::setParameters(FMaterial&& mat)
{
	parameters = std::move(mat);
}

void CMaterial::addTexture(const std::string& srTexture, size_t index)
{
	mTextures[srTexture] = index;
}

const size_t CMaterial::getTexture(const std::string& srTexture) const
{
	if (auto found = mTextures.find(srTexture); found != mTextures.end())
		return found->second;

	return invalid_index;
}

void CMaterial::setShader(size_t index)
{
	shader_id = index;
}

const size_t CMaterial::getShader() const
{
	return shader_id;
}

void CMaterial::incrementUsageCount()
{
	usageCount++;
}

const size_t CMaterial::getUsageCount() const
{
	return usageCount;
}