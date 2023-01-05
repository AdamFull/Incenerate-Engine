#pragma once
#include "shader/ShaderObject.h"
#include "image/Image.h"
#include "rendering/material/Material.h"
#include "buffers/VertexBufferObject.h"
#include "graphics/rendering/RenderStage.h"

namespace engine
{
	namespace graphics
	{
		class CResourceHolder
		{
		public:
			size_t addShader(std::unique_ptr<CShaderObject>&& shader);
			void removeShader(size_t id);
			const std::unique_ptr<CShaderObject>& getShader(size_t id);

			size_t addImage(std::unique_ptr<CImage>&& image);
			void removeImage(size_t id);
			const std::unique_ptr<CImage>& getImage(size_t id);

			size_t addMaterial(std::unique_ptr<CMaterial>&& material);
			void removeMaterial(size_t id);
			const std::unique_ptr<CMaterial>& getMaterial(size_t id);

			size_t addVertexBuffer(std::unique_ptr<CVertexBufferObject>&& vbo);
			void removeVertexBuffer(size_t id);
			const std::unique_ptr<CVertexBufferObject>& getVertexBuffer(size_t id);

			size_t addRenderStage(std::unique_ptr<CRenderStage>&& rs);
			void removeRenderStage(size_t id);
			const std::unique_ptr<CRenderStage>& getRenderStage(size_t id);
		private:
			std::map<size_t, std::unique_ptr<CShaderObject>> vShaders;
			std::map<size_t, std::unique_ptr<CImage>> vImages;
			std::map<size_t, std::unique_ptr<CMaterial>> vMaterials;
			std::map<size_t, std::unique_ptr<CVertexBufferObject>> vVBOs;
			std::map<size_t, std::unique_ptr<CRenderStage>> vRenderStages;
		};
	}
}