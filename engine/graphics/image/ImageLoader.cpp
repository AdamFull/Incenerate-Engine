#include "ImageLoader.h"
#include "filesystem/interface/VirtualFileSystemInterface.h"

#include <ktx.h>
#include <ktxvulkan.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "filesystem/vfs_helper.h"



using namespace engine::filesystem;
using namespace engine::graphics;

CImageLoader::CImageLoader(filesystem::IVirtualFileSystemInterface* vfs_ptr) :
    m_pVFS(vfs_ptr)
{

}

void CImageLoader::load(const std::string& fsPath, std::unique_ptr<FImageCreateInfo>& imageCI, bool header)
{
    imageCI = std::make_unique<FImageCreateInfo>();

    if (!m_pVFS->exists(fsPath))
        log_error("Image {} was not found.", fsPath);

    auto texFormat = getTextureFormat(fsPath);
    switch (texFormat)
    {
    case EImageFormat::eKTX: { loadKTX(fsPath, imageCI, header); } break;
    case EImageFormat::eKTX2: { loadKTX2(fsPath, imageCI, header); } break;
    case EImageFormat::eDDS: { loadDDS(fsPath, imageCI, header); } break;

    default: { loadSTB(fsPath, imageCI, header); } break;
    }
}

void CImageLoader::loadSTB_memory(const std::vector<uint8_t>& imgdata, std::unique_ptr<FImageCreateInfo>& imageCI, bool header)
{
    int width{ 0 }, height{ 0 }, channels{ 0 };
    auto data = stbi_loadf_from_memory(imgdata.data(), imgdata.size(), &width, &height, &channels, STBI_rgb_alpha);

    imageCI->baseWidth = width;
    imageCI->baseHeight = height;
    imageCI->baseDepth = 1;
    imageCI->dataSize = width * height * 4;

    if (!header)
    {
        imageCI->pData = std::make_unique<uint8_t[]>(imageCI->dataSize);
        std::memcpy(imageCI->pData.get(), data, imageCI->dataSize);
    }

    imageCI->isArray = false;
    imageCI->isCompressed = false;
    imageCI->isCubemap = false;
    imageCI->generateMipmaps = true;
    imageCI->numDimensions = 2;
    imageCI->numLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(width, height)))) + 1;
    imageCI->numLevels = 1;
    imageCI->numLayers = 1;
    imageCI->numFaces = 1;

    stbi_image_free(data);
}

void CImageLoader::loadSTB(const std::string& fsPath, std::unique_ptr<FImageCreateInfo>& imageCI, bool header)
{
    std::vector<uint8_t> imgdata;
    m_pVFS->readFile(fsPath, imgdata);
    loadSTB_memory(imgdata, imageCI, header);
}


void CImageLoader::loadKTX_memory(const std::vector<uint8_t>& imgdata, std::unique_ptr<FImageCreateInfo>& imageCI, bool header)
{
    ktxTexture* kTexture{ nullptr };
    auto ktxresult = ktxTexture_CreateFromMemory(imgdata.data(), imgdata.size(), KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT, &kTexture);
    log_cerror(ktxresult == KTX_SUCCESS, "Failed to open ktx texture.");

    imageCI->isArray = kTexture->isArray;
    imageCI->isCubemap = kTexture->isCubemap;
    imageCI->isCompressed = kTexture->isCompressed;
    imageCI->generateMipmaps = kTexture->generateMipmaps;
    imageCI->baseWidth = kTexture->baseWidth;
    imageCI->baseHeight = kTexture->baseHeight;
    imageCI->baseDepth = kTexture->baseDepth;
    imageCI->numDimensions = kTexture->numDimensions;
    imageCI->numLevels = kTexture->numLevels;
    imageCI->numLayers = kTexture->numLayers;
    imageCI->numFaces = kTexture->numFaces;

    imageCI->dataSize = kTexture->dataSize;

    if (!header)
    {
        imageCI->pData = std::make_unique<uint8_t[]>(imageCI->dataSize);
        std::memcpy(imageCI->pData.get(), kTexture->pData, imageCI->dataSize);
    }

    imageCI->pixFormat = static_cast<vk::Format>(ktxTexture_GetVkFormat(kTexture));

    auto numLayers = imageCI->isCubemap ? imageCI->numFaces : imageCI->numLayers;
    for (uint32_t layer = 0; layer < numLayers; layer++)
    {
        for (uint32_t level = 0; level < imageCI->numLevels; level++)
        {
            size_t offset{ 0 };
            ktxresult = ktxTexture_GetImageOffset(kTexture, level, 0, layer, &offset);
            log_cerror(ktxresult == KTX_SUCCESS, "Failed to load mip level.");
            imageCI->mipOffsets[layer].emplace_back(offset);
        }
    }

    ktxTexture_Destroy(kTexture);
}

void CImageLoader::loadKTX(const std::string& fsPath, std::unique_ptr<FImageCreateInfo>& imageCI, bool header)
{
    std::vector<uint8_t> imgdata;
    m_pVFS->readFile(fsPath, imgdata);
    loadKTX_memory(imgdata, imageCI, header);
}


void CImageLoader::loadKTX2_memory(const std::vector<uint8_t>& imgdata, std::unique_ptr<FImageCreateInfo>& imageCI, bool header)
{
    ktxTexture2* kTexture{ nullptr };
    auto ktxresult = ktxTexture_CreateFromMemory(imgdata.data(), imgdata.size(), KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT, (ktxTexture**)&kTexture);
    log_cerror(ktxresult == KTX_SUCCESS, "Failed to open ktx texture.");

    imageCI->isArray = kTexture->isArray;
    imageCI->isCubemap = kTexture->isCubemap;
    imageCI->isCompressed = kTexture->isCompressed;
    imageCI->generateMipmaps = kTexture->generateMipmaps;
    imageCI->baseWidth = kTexture->baseWidth;
    imageCI->baseHeight = kTexture->baseHeight;
    imageCI->baseDepth = kTexture->baseDepth;
    imageCI->numDimensions = kTexture->numDimensions;
    imageCI->numLevels = kTexture->numLevels;
    imageCI->numLayers = kTexture->numLayers;
    imageCI->numFaces = kTexture->numFaces;

    if (!header)
    {
        if (ktxTexture2_NeedsTranscoding(kTexture))
        {
            ktxresult = ktxTexture2_TranscodeBasis(kTexture, KTX_TTF_BC7_RGBA, 0);
            log_cerror(ktxresult == KTX_SUCCESS, "Failed to transcode texture.");
        }

        imageCI->dataSize = kTexture->dataSize;
        imageCI->pData = std::make_unique<uint8_t[]>(imageCI->dataSize);
        std::memcpy(imageCI->pData.get(), kTexture->pData, imageCI->dataSize);
    }

    imageCI->pixFormat = static_cast<vk::Format>(kTexture->vkFormat);

    auto numLayers = imageCI->isCubemap ? imageCI->numFaces : imageCI->numLayers;
    for (uint32_t layer = 0; layer < numLayers; layer++)
    {
        for (uint32_t level = 0; level < imageCI->numLevels; level++)
        {
            size_t offset{ 0 };
            ktxresult = ktxTexture_GetImageOffset((ktxTexture*)kTexture, level, 0, layer, &offset);
            log_cerror(ktxresult == KTX_SUCCESS, "Failed to load mip level.");
            imageCI->mipOffsets[layer].emplace_back(offset);
        }
    }

    ktxTexture_Destroy((ktxTexture*)kTexture);
}

void CImageLoader::loadKTX2(const std::string& fsPath, std::unique_ptr<FImageCreateInfo>& imageCI, bool header)
{
    std::vector<uint8_t> imgdata;
    m_pVFS->readFile(fsPath, imgdata);
    loadKTX2_memory(imgdata, imageCI, header);
}

void CImageLoader::loadDDS(const std::string& fsPath, std::unique_ptr<FImageCreateInfo>& imageCI, bool header)
{

}

EImageFormat CImageLoader::getTextureFormat(const std::string& fsPath)
{
    static std::map<std::string, EImageFormat> imageFormatMapStatic
    {
        {".ktx", EImageFormat::eKTX},
        {".ktx2", EImageFormat::eKTX2},
        {".dds", EImageFormat::eDDS}
    };

    auto ext = fs::get_ext(fsPath);
    auto it = imageFormatMapStatic.find(ext);
    if (it != imageFormatMapStatic.end())
        return it->second;

    return EImageFormat::eUnknown;
}