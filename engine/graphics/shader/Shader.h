#pragma once

#include "ShaderReflect.h"

namespace engine
{
	namespace graphics
	{
		struct FSpecializationConstantData
		{
			std::vector<vk::SpecializationMapEntry> entries{};
			vk::SpecializationInfo info{};
		};

		class CShader : public CShaderReflect
		{
		public:
			CShader(CDevice* device);
			virtual ~CShader();

			void create(const std::string& name);

			void addStage(const std::vector<uint32_t>& spirv, vk::ShaderStageFlagBits stage) override;
			void buildReflection() override;

			const std::unordered_map < uint32_t, std::vector<vk::DescriptorSetLayoutBinding>>& getDescriptorSetLayouts() const { return mDescriptorSetLayouts; }
			const std::vector<vk::DescriptorPoolSize>& getDescriptorPools() const { return vDescriptorPools; }
			const std::vector<vk::PipelineShaderStageCreateInfo>& getStageCreateInfo() const { return vShaderModules; }
			std::vector<vk::PushConstantRange> getPushConstantRanges() const;

			CDevice* getDevice();

		private:
			void createShaderModule(const std::vector<uint32_t>& spirv, vk::ShaderStageFlagBits stage);
		private:
			CDevice* pDevice{ nullptr };

			std::string srShaderName{};

			std::unordered_map<uint32_t, std::vector<vk::DescriptorSetLayoutBinding>> mDescriptorSetLayouts;
			std::vector<vk::DescriptorPoolSize> vDescriptorPools;

			std::vector<vk::PipelineShaderStageCreateInfo> vShaderModules;

			std::unordered_map<vk::ShaderStageFlagBits, FSpecializationConstantData> mSpecConstantData{};
		};
	}
}