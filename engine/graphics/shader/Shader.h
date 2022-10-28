#pragma once

#include "ShaderReflect.h"

namespace engine
{
	namespace graphics
	{
		class CShader : public CShaderReflect
		{
		public:
			CShader(CDevice* device);
			virtual ~CShader();

			void addStage(const std::vector<uint32_t>& spirv, vk::ShaderStageFlagBits stage) override;
			void buildReflection() override;

			const std::vector<vk::DescriptorSetLayoutBinding>& getDescriptorSetLayouts() const { return vDescriptorSetLayouts; }
			const std::vector<vk::DescriptorPoolSize>& getDescriptorPools() const { return vDescriptorPools; }
			const std::vector<vk::PipelineShaderStageCreateInfo>& getStageCreateInfo() const { return vShaderModules; }
			std::vector<vk::PushConstantRange> getPushConstantRanges() const;

		private:
			void createShaderModule(const std::vector<uint32_t>& spirv, vk::ShaderStageFlagBits stage);
		private:
			CDevice* pDevice{ nullptr };

			std::vector<vk::DescriptorSetLayoutBinding> vDescriptorSetLayouts;
			uint32_t lastDescriptorBinding = 0;
			std::vector<vk::DescriptorPoolSize> vDescriptorPools;

			std::vector<vk::PipelineShaderStageCreateInfo> vShaderModules;
		};
	}
}