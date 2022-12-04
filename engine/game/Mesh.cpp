#include "Mesh.h"

#include "Engine.h"

using namespace engine::game;

void FMeshlet::setDimensions(glm::vec3 min, glm::vec3 max)
{
	dimensions.min = min;
	dimensions.max = max;
	dimensions.size = max - min;
	dimensions.center = (min + max) / 2.0f;
	dimensions.radius = glm::distance(min, max) / 2.0f;
}

void CMesh::create()
{

}

void CMesh::reCreate()
{

}

void CMesh::render()
{
	auto& renderSystem = CEngine::getInstance()->getGraphics()->getRenderSystem();
	if (renderSystem)
	{
		auto model = getModel();
		auto normal = glm::transpose(glm::inverse(model));
		auto position = getPosition();
		auto& frustum = renderSystem->getFrustum();

		for (auto& meshlet : vMeshlets)
		{
			bool needToRender{ true };
			if (bEnableCulling)
			{
				switch (eCullingType)
				{
				case ECullingType::eByPoint:
					needToRender = frustum.checkPoint(position);
					break;
				case ECullingType::eBySphere:
					needToRender = frustum.checkSphere(position, meshlet.dimensions.radius);
					break;
				case ECullingType::eByBox:
					needToRender = frustum.checkBox(position + meshlet.dimensions.min * transform.getScale(), position + meshlet.dimensions.max * transform.getScale());
					break;
				}
			}

			meshlet.bWasCulled = needToRender;

			//renderSystem->addMeshlet(meshlet);
		}
	}
}

void CMesh::update(float fDeltaTime)
{

}

void CMesh::addMeshlet(FMeshlet&& meshlet)
{
	vMeshlets.emplace_back(std::move(meshlet));
}