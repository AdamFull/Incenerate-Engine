#include "ImageLoader.h"

#include <ktx.h>
#include <ktxvulkan.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "system/filesystem/filesystem.h"



using namespace engine::system;
using namespace engine::loaders;

void CImageLoader::load(const std::filesystem::path& fsPath, std::unique_ptr<FImageCreateInfo>& imageCI, bool header)
{
    imageCI = std::make_unique<FImageCreateInfo>();

    auto fsFullPath = fs::get_workdir() / fsPath;

    if (!fs::is_file_exists(fsFullPath))
        fsFullPath = fs::get_workdir(true) / fsPath;

    if (!fs::is_file_exists(fsFullPath))
        log_error("Image {} was not found.", fsPath.string());

    auto texFormat = getTextureFormat(fsPath);
    switch (texFormat)
    {
    case EImageFormat::eKTX: { loadKTX(fsFullPath, imageCI, header); } break;
    case EImageFormat::eKTX2: { loadKTX2(fsFullPath, imageCI, header); } break;
    case EImageFormat::eDDS: { loadDDS(fsFullPath, imageCI, header); } break;

    default: { loadSTB(fsFullPath, imageCI, header); } break;
    }
}

void CImageLoader::loadSTB(const std::filesystem::path& fsPath, std::unique_ptr<FImageCreateInfo>& imageCI, bool header)
{
    auto filename = fs::from_unicode(fsPath);

    int width{ 0 }, height{ 0 }, channels{ 0 };
    auto data = stbi_load(filename.c_str(), &width, &height, &channels, STBI_rgb_alpha);

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

void CImageLoader::loadKTX(const std::filesystem::path& fsPath, std::unique_ptr<FImageCreateInfo>& imageCI, bool header)
{
    ktxTexture* kTexture{ nullptr };

    auto ktxresult = ktxTexture_CreateFromNamedFile(fs::from_unicode(fsPath).c_str(), KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT, &kTexture);
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

void CImageLoader::loadKTX2(const std::filesystem::path& fsPath, std::unique_ptr<FImageCreateInfo>& imageCI, bool header)
{
    ktxTexture2* kTexture{ nullptr };

    auto ktxresult = ktxTexture_CreateFromNamedFile(fs::from_unicode(fsPath).c_str(), KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT, (ktxTexture**)&kTexture);
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

void CImageLoader::loadDDS(const std::filesystem::path& fsPath, std::unique_ptr<FImageCreateInfo>& imageCI, bool header)
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

    auto ext = fs::from_unicode(fsPath.extension());
    auto it = imageFormatMapStatic.find(ext);
    if (it != imageFormatMapStatic.end())
        return it->second;

    return EImageFormat::eUnknown;
}