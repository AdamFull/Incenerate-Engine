#pragma once

#include "Shader.h"
#include "pipeline/Pipeline.h"
#include "APIStructures.h"

namespace engine
{
	namespace graphics
	{
		class CShaderObject
		{
		public:
			CShaderObject() = default;

			vk::CullModeFlagBits getCullMode() { return programCI.cullMode; }
			vk::FrontFace getFrontFace() { return programCI.frontFace; }
			bool getDepthTestFlag() { return programCI.depthTest; }
			std::vector<vk::DynamicState>& getDynamicStateEnables() { return programCI.dynamicStates; }
			bool getTesselationFlag() { return programCI.tesselation; }

			const std::unique_ptr<CShader>& getShader();
		private:
			std::unique_ptr<CShader> pShader;
			std::unique_ptr<CPipeline> pPipeline;
			FProgramCreateInfo programCI;
		};
	}
}

//getCullMode();
//getFrontFace();
//getDepthTestFlag();
//getDynamicStateEnables();
//getTesselationFlag();