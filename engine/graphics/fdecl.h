#pragma once

namespace engine
{
	namespace graphics
	{
		enum class EAlphaMode;

		class CAPIHandle;
		class CDevice;
		class CFramebuffer;

		class CShader;
		class CShaderCompiler;
		class CShaderLoader;
		class CShaderObject;
		class CShaderReflect;

		class CPipeline;
		class CRayTracePipeline;
		class CGraphicsPipeline;
		class CComputePipeline;

		class CImage;
		class CImage2D;
		class CImage2DArray;
		class CImageCubemap;
		class CImageCubemapArray;

		class CHandler;
		class CPushHandler;
		class CStorageHandler;
		class CUniformHandler;

		class CDescriptorSet;
		class CDescriptorHandler;

		class CBuffer;
		class CCommandBuffer;
		class CCommandPool;
		class CVertexBufferObject;
	}
}