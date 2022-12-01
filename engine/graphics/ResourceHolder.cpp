#include "ResourceHolder.h"

using namespace engine::graphics;

template<class _Cont, class _Ty>
size_t emplace_to(_Cont& container, _Ty&& object)
{
	container.emplace(container.size(), std::forward<_Ty>(object));
	return container.size() - 1;
}

size_t CResourceHolder::addShader(std::unique_ptr<CShaderObject>&& shader)
{
	return emplace_to(vShaders, std::move(shader));
}

size_t CResourceHolder::addImage(std::unique_ptr<CImage>&& image)
{
	return emplace_to(vImages, std::move(image));
}

size_t CResourceHolder::addMaterial(std::unique_ptr<CMaterial>&& material)
{
	return emplace_to(vMaterials, std::move(material));
}

size_t CResourceHolder::addVertexBuffer(std::unique_ptr<CVertexBufferObject>&& vbo)
{
	return emplace_to(vVBOs, std::move(vbo));
}