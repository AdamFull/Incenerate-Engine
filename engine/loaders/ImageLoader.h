#pragma once

#include <vulkan/vulkan.hpp>

namespace engine
{
	namespace loaders
	{
		enum class EImageFormat
		{
			eUnknown,

			eKTX,
			eKTX2,
			eDDS
		};

        struct FImageCreateInfo
        {
            bool isArray{ false };
            bool isCubemap{ false };
            bool isCompressed{ false };
            bool generateMipmaps{ false };

            uint32_t baseWidth{ 0U };
            uint32_t baseHeight{ 0U };
            uint32_t baseDepth{ 0U };
            uint32_t numDimensions{ 0U };
            uint32_t numLevels{ 0U };
            uint32_t numLayers{ 0U };
            uint32_t numFaces{ 0U };

            vk::Format pixFormat{};

            std::map<uint32_t, std::vector<size_t>> mipOffsets;

            size_t dataSize{ 0UL };
            std::unique_ptr<uint8_t[]> pData{ nullptr };
        };

        class CImageLoader
        {
        public:
            static void load(const std::filesystem::path& fsPath, std::unique_ptr<FImageCreateInfo>& imageCI, bool header = false);
        private:
            static void loadSTB(const std::filesystem::path& fsPath, std::unique_ptr<FImageCreateInfo>& imageCI, bool header);
            static void loadKTX(const std::filesystem::path& fsPath, std::unique_ptr<FImageCreateInfo>& imageCI, bool header);
            static void loadKTX2(const std::filesystem::path& fsPath, std::unique_ptr<FImageCreateInfo>& imageCI, bool header);
            static void loadDDS(const std::filesystem::path& fsPath, std::unique_ptr<FImageCreateInfo>& imageCI, bool header);
            static EImageFormat getTextureFormat(const std::filesystem::path& fsPath);
        };
	}
}