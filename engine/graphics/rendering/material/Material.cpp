#include "Material.h"

#include "Engine.h"

using namespace engine::graphics;

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
	iShader = index;
}

const size_t CMaterial::getShader() const
{
	return iShader;
}

void CMaterial::incrementUsageCount()
{
	usageCount++;
}

const size_t CMaterial::getUsageCount() const
{
	return usageCount;
}