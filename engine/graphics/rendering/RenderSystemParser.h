#pragma once

#include "EngineStructures.h"

namespace engine
{
	namespace graphics
	{
		struct FJSONViewport
		{
			std::string srFrom{"0x0"};
			std::string srTo{"__FULLSCREEN__"};
		};

		void from_json(const nlohmann::json& json, FJSONViewport& type);

		struct FJSONImage
		{
			std::string srName;
			std::string srFormat;
			std::string srUsage;
			EImageType type{ EImageType::e2D };
			uint32_t layers{ 1 };
		};

		void from_json(const nlohmann::json& json, FJSONImage& type);

		struct FJSONDependencyDesc
		{
			std::string srSubpass;
			std::string srStageMask;
			std::string srAccessMask;
		};

		void from_json(const nlohmann::json& json, FJSONDependencyDesc& type);

		struct FJSONDependency
		{
			FJSONDependencyDesc src;
			FJSONDependencyDesc dst;
			std::string srDepFlags{ "ByRegion" };
		};

		void from_json(const nlohmann::json& json, FJSONDependency& type);

		struct FJSONStage
		{
			std::string srName;
			FJSONViewport viewport;
			bool bFlipViewport{ false };
			std::vector<FJSONImage> vImages;
			std::vector<std::string> vOutputs;
			std::vector<std::string> vDescriptions;
			std::vector<FJSONDependency> vDependencies;
		};

		void from_json(const nlohmann::json& json, FJSONStage& type);

		struct FJSONRenderSystem
		{
			std::string srName;
			std::vector<FJSONStage> vStages;
		};

		void from_json(const nlohmann::json& json, FJSONRenderSystem& type);

		//-------------------------------------------------------------------------------------

		struct FCIViewport
		{
			vk::Offset2D offset;
			vk::Extent2D extent;
		};

		struct FCIImage
		{
			std::string srName;
			vk::Format format;
			vk::ImageUsageFlags usage;
			EImageType type{ EImageType::e2D };
			uint32_t layers{ 1 };
		};

		struct FCIDependencyDesc
		{
			uint32_t subpass;
			vk::PipelineStageFlags stageMask; 
			vk::AccessFlags accessMask;
		};

		struct FCIDependency
		{
			FCIDependencyDesc src;
			FCIDependencyDesc dst;
			vk::DependencyFlags depFlags{ vk::DependencyFlagBits::eByRegion };
		};

		struct FCIStage
		{
			std::string srName;
			FCIViewport viewport;
			bool bFlipViewport{ false };
			std::vector<FCIImage> vImages;
			std::vector<std::string> vOutputs;
			std::vector<std::string> vDescriptions;
			std::vector<FCIDependency> vDependencies;
		};

		struct FCIRenderSystem
		{
			std::string srName;
			std::vector<FCIStage> vStages;
		};
		
		class CRenderSystemParser
		{
		public:
			CRenderSystemParser(CDevice* device);
			FCIRenderSystem parse(const std::string& path);
			FCIRenderSystem reParse();

		private:
			CDevice* pDevice{ nullptr };
			std::hash<std::string> hasher;
			std::string srConfigPath;

			FCIViewport parseViewport(const FJSONViewport& viewport);
			vk::Offset2D parseOffset(const std::string& offset);
			vk::Extent2D parseExtent(const std::string& extent);
			std::vector<FCIImage> parseImages(const std::vector<FJSONImage>& images);
			vk::Format parseFormat(const std::string& srFormat);
			vk::ImageUsageFlags parseUsage(const std::string& srUsage);
			std::vector<FCIDependency> parseDependencies(const std::vector<FJSONDependency>& dependencies);
			FCIDependencyDesc parseDependencyDesc(const FJSONDependencyDesc& dependencyDesc);
			uint32_t parseSubpass(const std::string& subpass);
			vk::PipelineStageFlags parseStageMask(const std::string& stageMask);
			vk::AccessFlags parseAccessMask(const std::string& accessMask);
			vk::DependencyFlags parseDependencyFlags(const std::string& flags);

		private:

			template<class _Map, class _Ty>
			void parseFlags(const std::string& input, const _Map& parsemap, _Ty& flagtype, char separator = '|')
			{
				for (auto& token : tokenize(input, separator))
				{
					auto found = parsemap.find(hasher(token));
					if (found != parsemap.end())
						flagtype |= found->second;
				}
			}

			static std::vector<std::string> tokenize(const std::string& input, char delimeter);
			static bool is_digit(const std::string& input);
		};
	}
}