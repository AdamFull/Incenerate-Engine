#pragma once

#include "DescriptorSet.h"

namespace engine
{
	namespace graphics
	{
		class CDescriptorHandler
		{
		public:
			CDescriptorHandler(CDevice* device);

			void create(CShaderObject* pPipeline);
			void update();
			void bind(const vk::CommandBuffer& commandBuffer) const;
			void reset();

			void set(const std::string& srUniformName, vk::DescriptorBufferInfo& bufferInfo);
			void set(const std::string& srUniformName, vk::DescriptorImageInfo& imageInfo);
			void set(const std::string& srUniformName, vk::WriteDescriptorSet& writeInfo);

			CDevice* getDevice();
		private:
			CDevice* pDevice{ nullptr };
			CShaderObject* pShaderObject{ nullptr };
			std::unique_ptr<CDescriptorSet> pDescriptorSet;
			std::vector<vk::WriteDescriptorSet> vWriteDescriptorSets;
		};
	}
}