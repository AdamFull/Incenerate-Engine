#include "ResourceHolder.h"

#include <atomic>

using namespace engine::graphics;

template<class _Cont, class _Ty>
size_t emplace_to(_Cont& container, _Ty&& object)
{
	static std::atomic<size_t> uid{ 0 };
	uid.fetch_add(1, std::memory_order_relaxed);

	if(uid == invalid_index)
		uid.fetch_add(1, std::memory_order_relaxed);

	container.emplace(uid, std::forward<_Ty>(object));
	return uid;
}

template<class _Ty, class _Cont>
const std::unique_ptr<_Ty>& get_from(_Cont& container, size_t id)
{
	auto found = container.find(id);
	if (found != container.end())
		return found->second;
	return nullptr;
}

size_t CResourceHolder::addShader(std::unique_ptr<CShaderObject>&& shader)
{
	return emplace_to(vShaders, std::move(shader));
}

void CResourceHolder::removeShader(size_t id)
{
	vShaders.erase(id);
}

const std::unique_ptr<CShaderObject>& CResourceHolder::getShader(size_t id)
{
	return get_from<CShaderObject>(vShaders, id);
}

size_t CResourceHolder::addImage(std::unique_ptr<CImage>&& image)
{
	return emplace_to(vImages, std::move(image));
}

void CResourceHolder::removeImage(size_t id)
{
	vImages.erase(id);
}

const std::unique_ptr<CImage>& CResourceHolder::getImage(size_t id)
{
	return get_from<CImage>(vImages, id);
}

size_t CResourceHolder::addMaterial(std::unique_ptr<CMaterial>&& material)
{
	return emplace_to(vMaterials, std::move(material));
}

void CResourceHolder::removeMaterial(size_t id)
{
	vMaterials.erase(id);
}

const std::unique_ptr<CMaterial>& CResourceHolder::getMaterial(size_t id)
{
	return get_from<CMaterial>(vMaterials, id);
}

size_t CResourceHolder::addVertexBuffer(std::unique_ptr<CVertexBufferObject>&& vbo)
{
	return emplace_to(vVBOs, std::move(vbo));
}

void CResourceHolder::removeVertexBuffer(size_t id)
{
	vVBOs.erase(id);
}

const std::unique_ptr<CVertexBufferObject>& CResourceHolder::getVertexBuffer(size_t id)
{
	return get_from<CVertexBufferObject>(vVBOs, id);
}

size_t CResourceHolder::addRenderStage(std::unique_ptr<CRenderStage>&& rs)
{
	return emplace_to(vRenderStages, std::move(rs));
}

void CResourceHolder::removeRenderStage(size_t id)
{
	vRenderStages.erase(id);
}

const std::unique_ptr<CRenderStage>& CResourceHolder::getRenderStage(size_t id)
{
	return get_from<CRenderStage>(vRenderStages, id);
}