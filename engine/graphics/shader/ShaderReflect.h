#pragma once

#include <vulkan/vulkan.hpp>
#include <spirvcross/spirv_cross.hpp>
#include <spirvcross/spirv_glsl.hpp>
#include <variant>

namespace engine
{
	namespace graphics
	{
		class CUniform
		{
		public:
			friend class CShaderReflect;
			CUniform() = default;

			uint32_t getSet() const { return set; }
			uint32_t getBinding() const { return binding; }
			std::size_t getOffset() const { return offset; }
			std::size_t getSize() const { return size; }
			vk::DescriptorType getDescriptorType() const { return descriptorType; }
			vk::ShaderStageFlags getStageFlags() const { return stageFlags; }

		private:
			uint32_t set{ 0 };
			uint32_t binding{ 0 };
			std::size_t offset{ 0 };
			std::size_t size{ 0 };
			vk::DescriptorType descriptorType;
			vk::ShaderStageFlags stageFlags{ vk::ShaderStageFlagBits::eAll };
		};

		class CUniformBlock
		{
		public:
			friend class CShaderReflect;
			CUniformBlock() = default;

			uint32_t getSet() const { return set; }
			uint32_t getBinding() const { return binding; }
			std::size_t getSize() const { return size; }
			vk::ShaderStageFlags getStageFlags() const { return stageFlags; }
			vk::DescriptorType getDescriptorType() const { return descriptorType; }
			const std::unordered_map<std::string, CUniform>& GetUniforms() const { return mUniforms; }

			const CUniform* getUniform(const std::string& name) const
			{
				if (const auto& it = mUniforms.find(name); it != mUniforms.end())
					return &it->second; 

				return nullptr;
			}

		private:
			uint32_t set{ 0 };
			uint32_t binding{ 0 };
			std::size_t size{ 0 };
			vk::ShaderStageFlags stageFlags{ vk::ShaderStageFlagBits::eAll };
			vk::DescriptorType descriptorType;
			std::unordered_map<std::string, CUniform> mUniforms{};
		};

		class CPushConstBlock
		{
		public:
			friend class CShaderReflect;
			CPushConstBlock() = default;

			std::size_t getSize() const { return size; }
			vk::ShaderStageFlags getStageFlags() const { return stageFlags; }
			const std::unordered_map<std::string, CUniform>& GetUniforms() const { return mUniforms; }
			const CUniform* getUniform(const std::string& name) const
			{
				if (const auto& it = mUniforms.find(name); it != mUniforms.end())
					return &it->second; 

				return nullptr;
			}

		private:
			std::size_t size{ 0 };
			vk::ShaderStageFlags stageFlags{ vk::ShaderStageFlagBits::eAll };
			std::unordered_map<std::string, CUniform> mUniforms{};
		};

		class CSpecializationConstant
		{
		public:
			friend class CShaderReflect;
			CSpecializationConstant() = default;

			uint32_t getConstantId() const { return constant_id; }
			std::size_t getSize() const { return size; }
			uint32_t getOffset() const { return offset; }
			vk::ShaderStageFlags getStageFlags() const { return stageFlags; }
		private:
			uint32_t constant_id{ 0 };
			std::size_t size{ 0 };
			uint32_t offset{ 0 };
			vk::ShaderStageFlags stageFlags{ vk::ShaderStageFlagBits::eAll };
		};

		class CShaderReflect
		{
		public:
			CShaderReflect() = default;
			virtual ~CShaderReflect() = default;

			virtual void addStage(const std::vector<uint32_t>& spirv, vk::ShaderStageFlagBits stage);
			virtual void buildReflection() {}

			void setDescriptorMultiplier(uint32_t multiplier) { descriptorMultiplier = multiplier; }

			//Getters
			const CUniform* getUniform(const std::string& name) const;
			const CUniformBlock* getUniformBlock(const std::string& name) const;
			const CPushConstBlock* getPushBlock(const std::string& name) const;

			const std::array<std::optional<uint32_t>, 3>& getLocalSizes(uint32_t index = 0) const { return localSizes[index]; }
			const uint32_t getControlPoints() const { return executionModeOutputVertices; }
			const std::unordered_map<std::string, CUniform>& getUniforms() const { return mUniforms; }
			const std::unordered_map<std::string, CUniformBlock>& getUniformBlocks() const { return mUniformBlocks; }
			const std::unordered_map<std::string, CPushConstBlock>& getPushBlocks() const { return mPushBlocks; }
			const std::unordered_map<std::string, CSpecializationConstant>& getSpecializationConstants() { return mSpecializationConstants; }

		protected:
			void processReflection(const std::vector<uint32_t>& spirv, vk::ShaderStageFlagBits stageFlag);

			static CUniformBlock buildUniformBlock(spirv_cross::CompilerGLSL* compiler, const spirv_cross::Resource& res, vk::ShaderStageFlagBits stageFlag, vk::DescriptorType descriptorType);
			static CUniform buildUnifrom(spirv_cross::CompilerGLSL* compiler, const spirv_cross::Resource& res, vk::ShaderStageFlagBits stageFlag, vk::DescriptorType descriptorType);
			static CPushConstBlock buildPushBlock(spirv_cross::CompilerGLSL* compiler, const spirv_cross::Resource& res, vk::ShaderStageFlagBits stageFlag);
			static CSpecializationConstant buildSpecConstant(spirv_cross::CompilerGLSL* compiler, const spirv_cross::SpecializationConstant& spec_const, vk::ShaderStageFlagBits stageFlag, uint32_t& offset, std::vector<char>& buffer);
			static std::size_t getTypeSize(spirv_cross::CompilerGLSL* compiler, const spirv_cross::SPIRType& type);
		protected:
			uint32_t descriptorMultiplier{ 1 };

			std::vector<std::array<std::optional<uint32_t>, 3>> localSizes;
			uint32_t executionModeInvocations{ 0 };
			uint32_t executionModeOutputVertices{ 0 };

			std::unordered_map<std::string, CUniform> mUniforms;
			std::unordered_map<std::string, CUniformBlock> mUniformBlocks;
			std::unordered_map<std::string, CPushConstBlock> mPushBlocks;

			std::unordered_map<std::string, CSpecializationConstant> mSpecializationConstants;
			std::vector<char> vSpecializationBuffer{};
		};
	}
}