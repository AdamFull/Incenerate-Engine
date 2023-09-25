#include "RayTracePipeline.h"

#include "APIHandle.h"
#include "shader/ShaderObject.h"

using namespace engine::graphics;

CRayTracePipeline::CRayTracePipeline(CDevice* device)
{
	pDevice = device;
}

void CRayTracePipeline::create(CShaderObject* pShader, vk::RenderPass& renderPass, const FShaderCreateInfo& specials)
{

}

void CRayTracePipeline::createPipeline(CShaderObject* pShader, const FShaderCreateInfo& specials)
{
	auto& program = pShader->getShader();
	auto shaderStages = program->getStageCreateInfo();

	std::vector<vk::RayTracingShaderGroupCreateInfoKHR> shaderGroups{};

	uint32_t shaderIndex{ 0 };
	for (auto& ci : shaderStages)
	{
		vk::RayTracingShaderGroupCreateInfoKHR shaderGroup{};
		switch (ci.stage)
		{
		case vk::ShaderStageFlagBits::eRaygenKHR: {
			shaderGroup.type = vk::RayTracingShaderGroupTypeKHR::eGeneral;
			shaderGroup.generalShader = shaderIndex;
			shaderGroup.closestHitShader = VK_SHADER_UNUSED_KHR;
			shaderGroup.anyHitShader = VK_SHADER_UNUSED_KHR;
			shaderGroup.intersectionShader = VK_SHADER_UNUSED_KHR;
		} break;
		case vk::ShaderStageFlagBits::eMissKHR: {
			shaderGroup.type = vk::RayTracingShaderGroupTypeKHR::eGeneral;
			shaderGroup.generalShader = shaderIndex;
			shaderGroup.closestHitShader = VK_SHADER_UNUSED_KHR;
			shaderGroup.anyHitShader = VK_SHADER_UNUSED_KHR;
			shaderGroup.intersectionShader = VK_SHADER_UNUSED_KHR;
		} break;
		case vk::ShaderStageFlagBits::eClosestHitKHR: {
			shaderGroup.type = vk::RayTracingShaderGroupTypeKHR::eTrianglesHitGroup;
			shaderGroup.generalShader = VK_SHADER_UNUSED_KHR;
			shaderGroup.closestHitShader = shaderIndex;
			shaderGroup.anyHitShader = VK_SHADER_UNUSED_KHR;
			shaderGroup.intersectionShader = VK_SHADER_UNUSED_KHR;
		} break;
		}
		
		shaderGroups.emplace_back(shaderGroup);
		shaderIndex++;
	}

	vk::RayTracingPipelineCreateInfoKHR rayTracingCI{};
	rayTracingCI.stageCount = static_cast<uint32_t>(shaderStages.size());
	rayTracingCI.pStages = shaderStages.data();
	rayTracingCI.groupCount = static_cast<uint32_t>(shaderGroups.size());
	rayTracingCI.pGroups = shaderGroups.data();
	rayTracingCI.maxPipelineRayRecursionDepth = 2;
	rayTracingCI.layout = pipelineLayout;
}