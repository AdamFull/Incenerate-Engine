#pragma once
#include "shader/ShaderObject.h"
#include "image/Image.h"
#include "rendering/material/Material.h"
#include "buffers/VertexBufferObject.h"

namespace engine
{
	namespace graphics
	{
		class CResourceHolder
		{
		public:
			size_t addShader(std::unique_ptr<CShaderObject>&& shader);
			size_t addImage(std::unique_ptr<CImage>&& image);
			size_t addMaterial(std::unique_ptr<CMaterial>&& material);
			size_t addVertexBuffer(std::unique_ptr<CVertexBufferObject>&& vbo);
		private:
			std::map<size_t, std::unique_ptr<CShaderObject>> vShaders;
			std::map<size_t, std::unique_ptr<CImage>> vImages;
			std::map<size_t, std::unique_ptr<CMaterial>> vMaterials;
			std::map<size_t, std::unique_ptr<CVertexBufferObject>> vVBOs;
		};
	}
}