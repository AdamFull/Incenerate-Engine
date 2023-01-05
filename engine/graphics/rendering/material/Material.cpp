#include "Material.h"

#include "Engine.h"

using namespace engine::graphics;

void CMaterial::setParameters(FMaterial&& mat)
{
	parameters = std::move(mat);
}

void CMaterial::setTexture(const std::string& srTexture, size_t index)
{
	mTextures[srTexture] = index;
}

size_t CMaterial::getTexture(const std::string& srTexture)
{
	if (auto found = mTextures.find(srTexture); found != mTextures.end())
		return found->second;

	return invalid_index;
}

void CMaterial::setShader(size_t index)
{
	iShader = index;
}

size_t CMaterial::getShader()
{
	return iShader;
}