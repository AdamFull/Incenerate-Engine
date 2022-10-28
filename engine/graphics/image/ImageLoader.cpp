#include "ImageLoader.h"

#include <utility/logger/logger.h>

#include <ktx.h>
#include <ktxvulkan.h>

using namespace engine::graphics;

void CImageLoader::load(const std::filesystem::path& fsPath, std::unique_ptr<FImageCreateInfo>& imageCI, const std::vector<vk::Format>& supportedFormats, const std::filesystem::path& parentPath)
{
    imageCI = std::make_unique<FImageCreateInfo>();

    auto fsFullPath = parentPath / fsPath;
    auto texFormat = getTextureFormat(fsPath);
    switch (texFormat)
    {
    case EImageFormat::eKTX: { loadKTX(fsFullPath, imageCI); } break;
    case EImageFormat::eKTX2: { loadKTX2(fsFullPath, imageCI, supportedFormats); } break;
    case EImageFormat::eDDS: { loadDDS(fsFullPath, imageCI); } break;

    default: break;
    }
}

void CImageLoader::loadKTX(const std::filesystem::path& fsPath, std::unique_ptr<FImageCreateInfo>& imageCI)
{
    ktxTexture* kTexture{ nullptr };

    auto ktxresult = ktxTexture_CreateFromNamedFile(fsPath.string().c_str(), KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT, &kTexture);
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
    imageCI->pData = std::make_unique<uint8_t[]>(imageCI->dataSize);
    std::memcpy(imageCI->pData.get(), kTexture->pData, imageCI->dataSize);

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

void CImageLoader::loadKTX2(const std::filesystem::path& fsPath, std::unique_ptr<FImageCreateInfo>& imageCI, const std::vector<vk::Format>& supportedFormats)
{
    ktxTexture2* kTexture{ nullptr };

    auto ktxresult = ktxTexture_CreateFromNamedFile(fsPath.string().c_str(), KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT, (ktxTexture**)&kTexture);
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

    if (ktxTexture2_NeedsTranscoding(kTexture))
    {
        ktxresult = ktxTexture2_TranscodeBasis(kTexture, KTX_TTF_BC7_RGBA, 0);
        log_cerror(ktxresult == KTX_SUCCESS, "Failed to transcode texture.");
    }

    imageCI->dataSize = kTexture->dataSize;
    imageCI->pData = std::make_unique<uint8_t[]>(imageCI->dataSize);
    std::memcpy(imageCI->pData.get(), kTexture->pData, imageCI->dataSize);

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

void CImageLoader::loadDDS(const std::filesystem::path& fsPath, std::unique_ptr<FImageCreateInfo>& imageCI)
{

}

EImageFormat CImageLoader::getTextureFormat(const std::filesystem::path& fsPath)
{
    static std::map<std::string, EImageFormat> imageFormatMapStatic
    {
        {".ktx", EImageFormat::eKTX},
        {".ktx2", EImageFormat::eKTX2},
        {".dds", EImageFormat::eDDS}
    };

    auto ext = fsPath.extension().string();
    auto it = imageFormatMapStatic.find(ext);
    if (it != imageFormatMapStatic.end())
        return it->second;

    return EImageFormat::eUnknown;
}