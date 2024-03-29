#pragma once

#include <vulkan/vulkan.hpp>

namespace engine
{
    namespace filesystem { class IVirtualFileSystemInterface; }
	namespace graphics
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
            CImageLoader(filesystem::IVirtualFileSystemInterface* vfs_ptr);
            void load(const std::string& fsPath, std::unique_ptr<FImageCreateInfo>& imageCI, bool header = false);
        private:
            void loadSTB_memory(const std::vector<uint8_t>& imgdata, std::unique_ptr<FImageCreateInfo>& imageCI, bool header);
            void loadSTB(const std::string& fsPath, std::unique_ptr<FImageCreateInfo>& imageCI, bool header);

            void loadKTX_memory(const std::vector<uint8_t>& imgdata, std::unique_ptr<FImageCreateInfo>& imageCI, bool header);
            void loadKTX(const std::string& fsPath, std::unique_ptr<FImageCreateInfo>& imageCI, bool header);

            void loadKTX2_memory(const std::vector<uint8_t>& imgdata, std::unique_ptr<FImageCreateInfo>& imageCI, bool header);
            void loadKTX2(const std::string& fsPath, std::unique_ptr<FImageCreateInfo>& imageCI, bool header);

            void loadDDS(const std::string& fsPath, std::unique_ptr<FImageCreateInfo>& imageCI, bool header);
            EImageFormat getTextureFormat(const std::string& fsPath);
        private:
            filesystem::IVirtualFileSystemInterface* m_pVFS{ nullptr };
        };
	}
}