#include "RenderSystemParser.h"
#include "system/filesystem/filesystem.h"
#include "graphics/APIHandle.h"

#include <sstream>

namespace engine
{
	namespace graphics
	{
		void from_json(const nlohmann::json& json, FJSONViewport& type)
		{
			utl::parse_to("from", json, type.srFrom);
			utl::parse_to("to", json, type.srTo);
		}

		void from_json(const nlohmann::json& json, FJSONImage& type)
		{
			utl::parse_to("name", json, type.srName);
			utl::parse_to("format", json, type.srFormat);
			utl::parse_to("usage", json, type.srUsage);
		}

		void from_json(const nlohmann::json& json, FJSONDependencyDesc& type)
		{
			utl::parse_to("subpass", json, type.srSubpass);
			utl::parse_to("stage_mask", json, type.srStageMask);
			utl::parse_to("access_mask", json, type.srAccessMask);
		}

		void from_json(const nlohmann::json& json, FJSONDependency& type)
		{
			utl::parse_to("src", json, type.src);
			utl::parse_to("dst", json, type.dst);
			utl::parse_to("flags", json, type.srDepFlags);
		}

		void from_json(const nlohmann::json& json, FJSONStage& type)
		{
			utl::parse_to("name", json, type.srName);
			utl::parse_to("viewport", json, type.viewport);
			utl::parse_to("flip", json, type.bFlipViewport);
			utl::parse_to("images", json, type.vImages);
			utl::parse_to("outputs", json, type.vOutputs);
			utl::parse_to("descriptions", json, type.vDescriptions);
			utl::parse_to("dependencies", json, type.vDependencies);
		}

		void from_json(const nlohmann::json& json, FJSONRenderSystem& type)
		{
			utl::parse_to("name", json, type.srName);
			utl::parse_to("stages", json, type.vStages);
		}
	}
}

using namespace engine::graphics;
using namespace engine::system;

CRenderSystemParser::CRenderSystemParser(CDevice* device)
{
	pDevice = device;
}

FCIRenderSystem CRenderSystemParser::parse(const std::string& path)
{
	srConfigPath = path;
	FCIRenderSystem renderSystemCI;

	FJSONRenderSystem renderSystemJSON;
	if (fs::read_json(path, renderSystemJSON))
	{
		renderSystemCI.srName = renderSystemJSON.srName;
		for (auto& stage : renderSystemJSON.vStages)
		{
			FCIStage stageCI;
			stageCI.srName = stage.srName;
			stageCI.viewport = parseViewport(stage.viewport);
			stageCI.bFlipViewport = stage.bFlipViewport;
			stageCI.vImages = parseImages(stage.vImages);
			stageCI.vOutputs = stage.vOutputs;
			stageCI.vDescriptions = stage.vDescriptions;
			stageCI.vDependencies = parseDependencies(stage.vDependencies);
			renderSystemCI.vStages.emplace_back(stageCI);
		}
	}

	return renderSystemCI;
}

FCIRenderSystem CRenderSystemParser::reParse()
{
	return parse(srConfigPath);
}

FCIViewport CRenderSystemParser::parseViewport(const FJSONViewport& viewport)
{
	FCIViewport viewportCI;
	viewportCI.offset = parseOffset(viewport.srFrom);
	viewportCI.extent = parseExtent(viewport.srTo);
	return viewportCI;
}

vk::Offset2D CRenderSystemParser::parseOffset(const std::string& offset)
{
	auto offsetHash = hasher(offset);
	vk::Offset2D offset2d;

	if (offsetHash == hasher("__FULLSCREEN__"))
	{
		auto fss = pDevice->getExtent();
		offset2d.x = fss.width;
		offset2d.y = fss.height;
	}
	else
	{
		auto tokens = tokenize(offset, 'x');
		if (!tokens.empty())
		{
			offset2d.x = std::stoul(tokens[0]);
			offset2d.y = std::stoul(tokens[1]);
		}
	}

	return offset2d;
}

vk::Extent2D CRenderSystemParser::parseExtent(const std::string& extent)
{
	auto extentHash = hasher(extent);
	vk::Extent2D extent2d;

	if (extentHash == hasher("__FULLSCREEN__"))
		extent2d = pDevice->getExtent();
	else
	{
		auto tokens = tokenize(extent, 'x');
		if (!tokens.empty())
		{
			extent2d.width = std::stoul(tokens[0]);
			extent2d.height = std::stoul(tokens[1]);
		}
	}

	return extent2d;
}

std::vector<FCIImage> CRenderSystemParser::parseImages(const std::vector<FJSONImage>& images)
{
	std::vector<FCIImage> imagesCI;

	for (auto& image : images)
	{
		FCIImage imageCI;
		imageCI.srName = image.srName;
		imageCI.format = parseFormat(image.srFormat);
		imageCI.usage = parseUsage(image.srUsage);
		imageCI.type = image.type;
		imageCI.layers = image.layers;
		imagesCI.emplace_back(imageCI);
	}

	return imagesCI;
}

vk::Format CRenderSystemParser::parseFormat(const std::string& srFormat)
{
	auto formatHash = hasher(srFormat);

	static std::map<size_t, std::function<vk::Format()>> mFormatMap
	{
		{ hasher("__PRESENT__"), [this]() { return pDevice->getImageFormat(); } },
		{ hasher("__DEPTH__"), [this]() { return pDevice->getDepthFormat(); } },
		{ hasher("Undefined"), []() { return vk::Format::eUndefined; } },
		{ hasher("R4G4UnormPack8"), []() { return vk::Format::eR4G4UnormPack8; } },
		{ hasher("R4G4B4A4UnormPack16"), []() { return vk::Format::eR4G4B4A4UnormPack16; } },
		{ hasher("B4G4R4A4UnormPack16"), []() { return vk::Format::eB4G4R4A4UnormPack16; } },
		{ hasher("R5G6B5UnormPack16"), []() { return vk::Format::eR5G6B5UnormPack16; } },
		{ hasher("B5G6R5UnormPack16"), []() { return vk::Format::eB5G6R5UnormPack16; } },
		{ hasher("R5G5B5A1UnormPack16"), []() { return vk::Format::eR5G5B5A1UnormPack16; } },
		{ hasher("B5G5R5A1UnormPack16"), []() { return vk::Format::eB5G5R5A1UnormPack16; } },
		{ hasher("A1R5G5B5UnormPack16"), []() { return vk::Format::eA1R5G5B5UnormPack16; } },
		{ hasher("R8Unorm"), []() { return vk::Format::eR8Unorm; } },
		{ hasher("R8Snorm"), []() { return vk::Format::eR8Snorm; } },
		{ hasher("R8Uscaled"), []() { return vk::Format::eR8Uscaled; } },
		{ hasher("R8Sscaled"), []() { return vk::Format::eR8Sscaled; } },
		{ hasher("R8Uint"), []() { return vk::Format::eR8Uint; } },
		{ hasher("R8Sint"), []() { return vk::Format::eR8Sint; } },
		{ hasher("R8Srgb"), []() { return vk::Format::eR8Srgb; } },
		{ hasher("R8G8Unorm"), []() { return vk::Format::eR8G8Unorm; } },
		{ hasher("R8G8Snorm"), []() { return vk::Format::eR8G8Snorm; } },
		{ hasher("R8G8Uscaled"), []() { return vk::Format::eR8G8Uscaled; } },
		{ hasher("R8G8Sscaled"), []() { return vk::Format::eR8G8Sscaled; } },
		{ hasher("R8G8Uint"), []() { return vk::Format::eR8G8Uint; } },
		{ hasher("R8G8Sint"), []() { return vk::Format::eR8G8Sint; } },
		{ hasher("R8G8Srgb"), []() { return vk::Format::eR8G8Srgb; } },
		{ hasher("R8G8B8Unorm"), []() { return vk::Format::eR8G8B8Unorm; } },
		{ hasher("R8G8B8Snorm"), []() { return vk::Format::eR8G8B8Snorm; } },
		{ hasher("R8G8B8Uscaled"), []() { return vk::Format::eR8G8B8Uscaled; } },
		{ hasher("R8G8B8Sscaled"), []() { return vk::Format::eR8G8B8Sscaled; } },
		{ hasher("R8G8B8Uint"), []() { return vk::Format::eR8G8B8Uint; } },
		{ hasher("R8G8B8Sint"), []() { return vk::Format::eR8G8B8Sint; } },
		{ hasher("R8G8B8Srgb"), []() { return vk::Format::eR8G8B8Srgb; } },
		{ hasher("B8G8R8Unorm"), []() { return vk::Format::eB8G8R8Unorm; } },
		{ hasher("B8G8R8Snorm"), []() { return vk::Format::eB8G8R8Snorm; } },
		{ hasher("B8G8R8Uscaled"), []() { return vk::Format::eB8G8R8Uscaled; } },
		{ hasher("B8G8R8Sscaled"), []() { return vk::Format::eB8G8R8Sscaled; } },
		{ hasher("B8G8R8Uint"), []() { return vk::Format::eB8G8R8Uint; } },
		{ hasher("B8G8R8Sint"), []() { return vk::Format::eB8G8R8Sint; } },
		{ hasher("B8G8R8Srgb"), []() { return vk::Format::eB8G8R8Srgb; } },
		{ hasher("R8G8B8A8Unorm"), []() { return vk::Format::eR8G8B8A8Unorm; } },
		{ hasher("R8G8B8A8Snorm"), []() { return vk::Format::eR8G8B8A8Snorm; } },
		{ hasher("R8G8B8A8Uscaled"), []() { return vk::Format::eR8G8B8A8Uscaled; } },
		{ hasher("R8G8B8A8Sscaled"), []() { return vk::Format::eR8G8B8A8Sscaled; } },
		{ hasher("R8G8B8A8Uint"), []() { return vk::Format::eR8G8B8A8Uint; } },
		{ hasher("R8G8B8A8Sint"), []() { return vk::Format::eR8G8B8A8Sint; } },
		{ hasher("R8G8B8A8Srgb"), []() { return vk::Format::eR8G8B8A8Srgb; } },
		{ hasher("B8G8R8A8Unorm"), []() { return vk::Format::eB8G8R8A8Unorm; } },
		{ hasher("B8G8R8A8Snorm"), []() { return vk::Format::eB8G8R8A8Snorm; } },
		{ hasher("B8G8R8A8Uscaled"), []() { return vk::Format::eB8G8R8A8Uscaled; } },
		{ hasher("B8G8R8A8Sscaled"), []() { return vk::Format::eB8G8R8A8Sscaled; } },
		{ hasher("B8G8R8A8Uint"), []() { return vk::Format::eB8G8R8A8Uint; } },
		{ hasher("B8G8R8A8Sint"), []() { return vk::Format::eB8G8R8A8Sint; } },
		{ hasher("B8G8R8A8Srgb"), []() { return vk::Format::eB8G8R8A8Srgb; } },
		{ hasher("A8B8G8R8UnormPack32"), []() { return vk::Format::eA8B8G8R8UnormPack32; } },
		{ hasher("A8B8G8R8SnormPack32"), []() { return vk::Format::eA8B8G8R8SnormPack32; } },
		{ hasher("A8B8G8R8UscaledPack32"), []() { return vk::Format::eA8B8G8R8UscaledPack32; } },
		{ hasher("A8B8G8R8SscaledPack32"), []() { return vk::Format::eA8B8G8R8SscaledPack32; } },
		{ hasher("A8B8G8R8UintPack32"), []() { return vk::Format::eA8B8G8R8UintPack32; } },
		{ hasher("A8B8G8R8SintPack32"), []() { return vk::Format::eA8B8G8R8SintPack32; } },
		{ hasher("A8B8G8R8SrgbPack32"), []() { return vk::Format::eA8B8G8R8SrgbPack32; } },
		{ hasher("A2R10G10B10UnormPack32"), []() { return vk::Format::eA2R10G10B10UnormPack32; } },
		{ hasher("A2R10G10B10SnormPack32"), []() { return vk::Format::eA2R10G10B10SnormPack32; } },
		{ hasher("A2R10G10B10UscaledPack32"), []() { return vk::Format::eA2R10G10B10UscaledPack32; } },
		{ hasher("A2R10G10B10SscaledPack32"), []() { return vk::Format::eA2R10G10B10SscaledPack32; } },
		{ hasher("A2R10G10B10UintPack32"), []() { return vk::Format::eA2R10G10B10UintPack32; } },
		{ hasher("A2R10G10B10SintPack32"), []() { return vk::Format::eA2R10G10B10SintPack32; } },
		{ hasher("A2B10G10R10UnormPack32"), []() { return vk::Format::eA2B10G10R10UnormPack32; } },
		{ hasher("A2B10G10R10SnormPack32"), []() { return vk::Format::eA2B10G10R10SnormPack32; } },
		{ hasher("A2B10G10R10UscaledPack32"), []() { return vk::Format::eA2B10G10R10UscaledPack32; } },
		{ hasher("A2B10G10R10SscaledPack32"), []() { return vk::Format::eA2B10G10R10SscaledPack32; } },
		{ hasher("A2B10G10R10UintPack32"), []() { return vk::Format::eA2B10G10R10UintPack32; } },
		{ hasher("A2B10G10R10SintPack32"), []() { return vk::Format::eA2B10G10R10SintPack32; } },
		{ hasher("R16Unorm"), []() { return vk::Format::eR16Unorm; } },
		{ hasher("R16Snorm"), []() { return vk::Format::eR16Snorm; } },
		{ hasher("R16Uscaled"), []() { return vk::Format::eR16Uscaled; } },
		{ hasher("R16Sscaled"), []() { return vk::Format::eR16Sscaled; } },
		{ hasher("R16Uint"), []() { return vk::Format::eR16Uint; } },
		{ hasher("R16Sint"), []() { return vk::Format::eR16Sint; } },
		{ hasher("R16Sfloat"), []() { return vk::Format::eR16Sfloat; } },
		{ hasher("R16G16Unorm"), []() { return vk::Format::eR16G16Unorm; } },
		{ hasher("R16G16Snorm"), []() { return vk::Format::eR16G16Snorm; } },
		{ hasher("R16G16Uscaled"), []() { return vk::Format::eR16G16Uscaled; } },
		{ hasher("R16G16Sscaled"), []() { return vk::Format::eR16G16Sscaled; } },
		{ hasher("R16G16Uint"), []() { return vk::Format::eR16G16Uint; } },
		{ hasher("R16G16Sint"), []() { return vk::Format::eR16G16Sint; } },
		{ hasher("R16G16Sfloat"), []() { return vk::Format::eR16G16Sfloat; } },
		{ hasher("R16G16B16Unorm"), []() { return vk::Format::eR16G16B16Unorm; } },
		{ hasher("R16G16B16Snorm"), []() { return vk::Format::eR16G16B16Snorm; } },
		{ hasher("R16G16B16Uscaled"), []() { return vk::Format::eR16G16B16Uscaled; } },
		{ hasher("R16G16B16Sscaled"), []() { return vk::Format::eR16G16B16Sscaled; } },
		{ hasher("R16G16B16Uint"), []() { return vk::Format::eR16G16B16Uint; } },
		{ hasher("R16G16B16Sint"), []() { return vk::Format::eR16G16B16Sint; } },
		{ hasher("R16G16B16Sfloat"), []() { return vk::Format::eR16G16B16Sfloat; } },
		{ hasher("R16G16B16A16Unorm"), []() { return vk::Format::eR16G16B16A16Unorm; } },
		{ hasher("R16G16B16A16Snorm"), []() { return vk::Format::eR16G16B16A16Snorm; } },
		{ hasher("R16G16B16A16Uscaled"), []() { return vk::Format::eR16G16B16A16Uscaled; } },
		{ hasher("R16G16B16A16Sscaled"), []() { return vk::Format::eR16G16B16A16Sscaled; } },
		{ hasher("R16G16B16A16Uint"), []() { return vk::Format::eR16G16B16A16Uint; } },
		{ hasher("R16G16B16A16Sint"), []() { return vk::Format::eR16G16B16A16Sint; } },
		{ hasher("R16G16B16A16Sfloat"), []() { return vk::Format::eR16G16B16A16Sfloat; } },
		{ hasher("R32Uint"), []() { return vk::Format::eR32Uint; } },
		{ hasher("R32Sint"), []() { return vk::Format::eR32Sint; } },
		{ hasher("R32Sfloat"), []() { return vk::Format::eR32Sfloat; } },
		{ hasher("R32G32Uint"), []() { return vk::Format::eR32G32Uint; } },
		{ hasher("R32G32Sint"), []() { return vk::Format::eR32G32Sint; } },
		{ hasher("R32G32Sfloat"), []() { return vk::Format::eR32G32Sfloat; } },
		{ hasher("R32G32B32Uint"), []() { return vk::Format::eR32G32B32Uint; } },
		{ hasher("R32G32B32Sint"), []() { return vk::Format::eR32G32B32Sint; } },
		{ hasher("R32G32B32Sfloat"), []() { return vk::Format::eR32G32B32Sfloat; } },
		{ hasher("R32G32B32A32Uint"), []() { return vk::Format::eR32G32B32A32Uint; } },
		{ hasher("R32G32B32A32Sint"), []() { return vk::Format::eR32G32B32A32Sint; } },
		{ hasher("R32G32B32A32Sfloat"), []() { return vk::Format::eR32G32B32A32Sfloat; } },
		{ hasher("R64Uint"), []() { return vk::Format::eR64Uint; } },
		{ hasher("R64Sint"), []() { return vk::Format::eR64Sint; } },
		{ hasher("R64Sfloat"), []() { return vk::Format::eR64Sfloat; } },
		{ hasher("R64G64Uint"), []() { return vk::Format::eR64G64Uint; } },
		{ hasher("R64G64Sint"), []() { return vk::Format::eR64G64Sint; } },
		{ hasher("R64G64Sfloat"), []() { return vk::Format::eR64G64Sfloat; } },
		{ hasher("R64G64B64Uint"), []() { return vk::Format::eR64G64B64Uint; } },
		{ hasher("R64G64B64Sint"), []() { return vk::Format::eR64G64B64Sint; } },
		{ hasher("R64G64B64Sfloat"), []() { return vk::Format::eR64G64B64Sfloat; } },
		{ hasher("R64G64B64A64Uint"), []() { return vk::Format::eR64G64B64A64Uint; } },
		{ hasher("R64G64B64A64Sint"), []() { return vk::Format::eR64G64B64A64Sint; } },
		{ hasher("R64G64B64A64Sfloat"), []() { return vk::Format::eR64G64B64A64Sfloat; } },
		{ hasher("B10G11R11UfloatPack32"), []() { return vk::Format::eB10G11R11UfloatPack32; } },
		{ hasher("E5B9G9R9UfloatPack32"), []() { return vk::Format::eE5B9G9R9UfloatPack32; } },
		{ hasher("D16Unorm"), []() { return vk::Format::eD16Unorm; } },
		{ hasher("X8D24UnormPack32"), []() { return vk::Format::eX8D24UnormPack32; } },
		{ hasher("D32Sfloat"), []() { return vk::Format::eD32Sfloat; } },
		{ hasher("S8Uint"), []() { return vk::Format::eS8Uint; } },
		{ hasher("D16UnormS8Uint"), []() { return vk::Format::eD16UnormS8Uint; } },
		{ hasher("D24UnormS8Uint"), []() { return vk::Format::eD24UnormS8Uint; } },
		{ hasher("D32SfloatS8Uint"), []() { return vk::Format::eD32SfloatS8Uint; } },
		{ hasher("Bc1RgbUnormBlock"), []() { return vk::Format::eBc1RgbUnormBlock; } },
		{ hasher("Bc1RgbSrgbBlock"), []() { return vk::Format::eBc1RgbSrgbBlock; } },
		{ hasher("Bc1RgbaUnormBlock"), []() { return vk::Format::eBc1RgbaUnormBlock; } },
		{ hasher("Bc1RgbaSrgbBlock"), []() { return vk::Format::eBc1RgbaSrgbBlock; } },
		{ hasher("Bc2UnormBlock"), []() { return vk::Format::eBc2UnormBlock; } },
		{ hasher("Bc2SrgbBlock"), []() { return vk::Format::eBc2SrgbBlock; } },
		{ hasher("Bc3UnormBlock"), []() { return vk::Format::eBc3UnormBlock; } },
		{ hasher("Bc3SrgbBlock"), []() { return vk::Format::eBc3SrgbBlock; } },
		{ hasher("Bc4UnormBlock"), []() { return vk::Format::eBc4UnormBlock; } },
		{ hasher("Bc4SnormBlock"), []() { return vk::Format::eBc4SnormBlock; } },
		{ hasher("Bc5UnormBlock"), []() { return vk::Format::eBc5UnormBlock; } },
		{ hasher("Bc5SnormBlock"), []() { return vk::Format::eBc5SnormBlock; } },
		{ hasher("Bc6HUfloatBlock"), []() { return vk::Format::eBc6HUfloatBlock; } },
		{ hasher("Bc6HSfloatBlock"), []() { return vk::Format::eBc6HSfloatBlock; } },
		{ hasher("Bc7UnormBlock"), []() { return vk::Format::eBc7UnormBlock; } },
		{ hasher("Bc7SrgbBlock"), []() { return vk::Format::eBc7SrgbBlock; } },
		{ hasher("Etc2R8G8B8UnormBlock"), []() { return vk::Format::eEtc2R8G8B8UnormBlock; } },
		{ hasher("Etc2R8G8B8SrgbBlock"), []() { return vk::Format::eEtc2R8G8B8SrgbBlock; } },
		{ hasher("Etc2R8G8B8A1UnormBlock"), []() { return vk::Format::eEtc2R8G8B8A1UnormBlock; } },
		{ hasher("Etc2R8G8B8A1SrgbBlock"), []() { return vk::Format::eEtc2R8G8B8A1SrgbBlock; } },
		{ hasher("Etc2R8G8B8A8UnormBlock"), []() { return vk::Format::eEtc2R8G8B8A8UnormBlock; } },
		{ hasher("Etc2R8G8B8A8SrgbBlock"), []() { return vk::Format::eEtc2R8G8B8A8SrgbBlock; } },
		{ hasher("EacR11UnormBlock"), []() { return vk::Format::eEacR11UnormBlock; } },
		{ hasher("EacR11SnormBlock"), []() { return vk::Format::eEacR11SnormBlock; } },
		{ hasher("EacR11G11UnormBlock"), []() { return vk::Format::eEacR11G11UnormBlock; } },
		{ hasher("EacR11G11SnormBlock"), []() { return vk::Format::eEacR11G11SnormBlock; } },
		{ hasher("Astc4x4UnormBlock"), []() { return vk::Format::eAstc4x4UnormBlock; } },
		{ hasher("Astc4x4SrgbBlock"), []() { return vk::Format::eAstc4x4SrgbBlock; } },
		{ hasher("Astc5x4UnormBlock"), []() { return vk::Format::eAstc5x4UnormBlock; } },
		{ hasher("Astc5x4SrgbBlock"), []() { return vk::Format::eAstc5x4SrgbBlock; } },
		{ hasher("Astc5x5UnormBlock"), []() { return vk::Format::eAstc5x5UnormBlock; } },
		{ hasher("Astc5x5SrgbBlock"), []() { return vk::Format::eAstc5x5SrgbBlock; } },
		{ hasher("Astc6x5UnormBlock"), []() { return vk::Format::eAstc6x5UnormBlock; } },
		{ hasher("Astc6x5SrgbBlock"), []() { return vk::Format::eAstc6x5SrgbBlock; } },
		{ hasher("Astc6x6UnormBlock"), []() { return vk::Format::eAstc6x6UnormBlock; } },
		{ hasher("Astc6x6SrgbBlock"), []() { return vk::Format::eAstc6x6SrgbBlock; } },
		{ hasher("Astc8x5UnormBlock"), []() { return vk::Format::eAstc8x5UnormBlock; } },
		{ hasher("Astc8x5SrgbBlock"), []() { return vk::Format::eAstc8x5SrgbBlock; } },
		{ hasher("Astc8x6UnormBlock"), []() { return vk::Format::eAstc8x6UnormBlock; } },
		{ hasher("Astc8x6SrgbBlock"), []() { return vk::Format::eAstc8x6SrgbBlock; } },
		{ hasher("Astc8x8UnormBlock"), []() { return vk::Format::eAstc8x8UnormBlock; } },
		{ hasher("Astc8x8SrgbBlock"), []() { return vk::Format::eAstc8x8SrgbBlock; } },
		{ hasher("Astc10x5UnormBlock"), []() { return vk::Format::eAstc10x5UnormBlock; } },
		{ hasher("Astc10x5SrgbBlock"), []() { return vk::Format::eAstc10x5SrgbBlock; } },
		{ hasher("Astc10x6UnormBlock"), []() { return vk::Format::eAstc10x6UnormBlock; } },
		{ hasher("Astc10x6SrgbBlock"), []() { return vk::Format::eAstc10x6SrgbBlock; } },
		{ hasher("Astc10x8UnormBlock"), []() { return vk::Format::eAstc10x8UnormBlock; } },
		{ hasher("Astc10x8SrgbBlock"), []() { return vk::Format::eAstc10x8SrgbBlock; } },
		{ hasher("Astc10x10UnormBlock"), []() { return vk::Format::eAstc10x10UnormBlock; } },
		{ hasher("Astc10x10SrgbBlock"), []() { return vk::Format::eAstc10x10SrgbBlock; } },
		{ hasher("Astc12x10UnormBlock"), []() { return vk::Format::eAstc12x10UnormBlock; } },
		{ hasher("Astc12x10SrgbBlock"), []() { return vk::Format::eAstc12x10SrgbBlock; } },
		{ hasher("Astc12x12UnormBlock"), []() { return vk::Format::eAstc12x12UnormBlock; } },
		{ hasher("Astc12x12SrgbBlock"), []() { return vk::Format::eAstc12x12SrgbBlock; } },
		{ hasher("G8B8G8R8422Unorm"), []() { return vk::Format::eG8B8G8R8422Unorm; } },
		{ hasher("B8G8R8G8422Unorm"), []() { return vk::Format::eB8G8R8G8422Unorm; } },
		{ hasher("G8B8R83Plane420Unorm"), []() { return vk::Format::eG8B8R83Plane420Unorm; } },
		{ hasher("G8B8R82Plane420Unorm"), []() { return vk::Format::eG8B8R82Plane420Unorm; } },
		{ hasher("G8B8R83Plane422Unorm"), []() { return vk::Format::eG8B8R83Plane422Unorm; } },
		{ hasher("G8B8R82Plane422Unorm"), []() { return vk::Format::eG8B8R82Plane422Unorm; } },
		{ hasher("G8B8R83Plane444Unorm"), []() { return vk::Format::eG8B8R83Plane444Unorm; } },
		{ hasher("R10X6UnormPack16"), []() { return vk::Format::eR10X6UnormPack16; } },
		{ hasher("R10X6G10X6Unorm2Pack16"), []() { return vk::Format::eR10X6G10X6Unorm2Pack16; } },
		{ hasher("R10X6G10X6B10X6A10X6Unorm4Pack16"), []() { return vk::Format::eR10X6G10X6B10X6A10X6Unorm4Pack16; } },
		{ hasher("G10X6B10X6G10X6R10X6422Unorm4Pack16"), []() { return vk::Format::eG10X6B10X6G10X6R10X6422Unorm4Pack16; } },
		{ hasher("B10X6G10X6R10X6G10X6422Unorm4Pack16"), []() { return vk::Format::eB10X6G10X6R10X6G10X6422Unorm4Pack16; } },
		{ hasher("G10X6B10X6R10X63Plane420Unorm3Pack16"), []() { return vk::Format::eG10X6B10X6R10X63Plane420Unorm3Pack16; } },
		{ hasher("G10X6B10X6R10X62Plane420Unorm3Pack16"), []() { return vk::Format::eG10X6B10X6R10X62Plane420Unorm3Pack16; } },
		{ hasher("G10X6B10X6R10X63Plane422Unorm3Pack16"), []() { return vk::Format::eG10X6B10X6R10X63Plane422Unorm3Pack16; } },
		{ hasher("G10X6B10X6R10X62Plane422Unorm3Pack16"), []() { return vk::Format::eG10X6B10X6R10X62Plane422Unorm3Pack16; } },
		{ hasher("G10X6B10X6R10X63Plane444Unorm3Pack16"), []() { return vk::Format::eG10X6B10X6R10X63Plane444Unorm3Pack16; } },
		{ hasher("R12X4UnormPack16"), []() { return vk::Format::eR12X4UnormPack16; } },
		{ hasher("R12X4G12X4Unorm2Pack16"), []() { return vk::Format::eR12X4G12X4Unorm2Pack16; } },
		{ hasher("R12X4G12X4B12X4A12X4Unorm4Pack16"), []() { return vk::Format::eR12X4G12X4B12X4A12X4Unorm4Pack16; } },
		{ hasher("G12X4B12X4G12X4R12X4422Unorm4Pack16"), []() { return vk::Format::eG12X4B12X4G12X4R12X4422Unorm4Pack16; } },
		{ hasher("B12X4G12X4R12X4G12X4422Unorm4Pack16"), []() { return vk::Format::eB12X4G12X4R12X4G12X4422Unorm4Pack16; } },
		{ hasher("G12X4B12X4R12X43Plane420Unorm3Pack16"), []() { return vk::Format::eG12X4B12X4R12X43Plane420Unorm3Pack16; } },
		{ hasher("G12X4B12X4R12X42Plane420Unorm3Pack16"), []() { return vk::Format::eG12X4B12X4R12X42Plane420Unorm3Pack16; } },
		{ hasher("G12X4B12X4R12X43Plane422Unorm3Pack16"), []() { return vk::Format::eG12X4B12X4R12X43Plane422Unorm3Pack16; } },
		{ hasher("G12X4B12X4R12X42Plane422Unorm3Pack16"), []() { return vk::Format::eG12X4B12X4R12X42Plane422Unorm3Pack16; } },
		{ hasher("G12X4B12X4R12X43Plane444Unorm3Pack16"), []() { return vk::Format::eG12X4B12X4R12X43Plane444Unorm3Pack16; } },
		{ hasher("G16B16G16R16422Unorm"), []() { return vk::Format::eG16B16G16R16422Unorm; } },
		{ hasher("B16G16R16G16422Unorm"), []() { return vk::Format::eB16G16R16G16422Unorm; } },
		{ hasher("G16B16R163Plane420Unorm"), []() { return vk::Format::eG16B16R163Plane420Unorm; } },
		{ hasher("G16B16R162Plane420Unorm"), []() { return vk::Format::eG16B16R162Plane420Unorm; } },
		{ hasher("G16B16R163Plane422Unorm"), []() { return vk::Format::eG16B16R163Plane422Unorm; } },
		{ hasher("G16B16R162Plane422Unorm"), []() { return vk::Format::eG16B16R162Plane422Unorm; } },
		{ hasher("G16B16R163Plane444Unorm"), []() { return vk::Format::eG16B16R163Plane444Unorm; } },
		{ hasher("G8B8R82Plane444Unorm"), []() { return vk::Format::eG8B8R82Plane444Unorm; } },
		{ hasher("G10X6B10X6R10X62Plane444Unorm3Pack16"), []() { return vk::Format::eG10X6B10X6R10X62Plane444Unorm3Pack16; } },
		{ hasher("G12X4B12X4R12X42Plane444Unorm3Pack16"), []() { return vk::Format::eG12X4B12X4R12X42Plane444Unorm3Pack16; } },
		{ hasher("G16B16R162Plane444Unorm"), []() { return vk::Format::eG16B16R162Plane444Unorm; } },
		{ hasher("A4R4G4B4UnormPack16"), []() { return vk::Format::eA4R4G4B4UnormPack16; } },
		{ hasher("A4B4G4R4UnormPack16"), []() { return vk::Format::eA4B4G4R4UnormPack16; } },
		{ hasher("Astc4x4SfloatBlock"), []() { return vk::Format::eAstc4x4SfloatBlock; } },
		{ hasher("Astc5x4SfloatBlock"), []() { return vk::Format::eAstc5x4SfloatBlock; } },
		{ hasher("Astc5x5SfloatBlock"), []() { return vk::Format::eAstc5x5SfloatBlock; } },
		{ hasher("Astc6x5SfloatBlock"), []() { return vk::Format::eAstc6x5SfloatBlock; } },
		{ hasher("Astc6x6SfloatBlock"), []() { return vk::Format::eAstc6x6SfloatBlock; } },
		{ hasher("Astc8x5SfloatBlock"), []() { return vk::Format::eAstc8x5SfloatBlock; } },
		{ hasher("Astc8x6SfloatBlock"), []() { return vk::Format::eAstc8x6SfloatBlock; } },
		{ hasher("Astc8x8SfloatBlock"), []() { return vk::Format::eAstc8x8SfloatBlock; } },
		{ hasher("Astc10x5SfloatBlock"), []() { return vk::Format::eAstc10x5SfloatBlock; } },
		{ hasher("Astc10x6SfloatBlock"), []() { return vk::Format::eAstc10x6SfloatBlock; } },
		{ hasher("Astc10x8SfloatBlock"), []() { return vk::Format::eAstc10x8SfloatBlock; } },
		{ hasher("Astc10x10SfloatBlock"), []() { return vk::Format::eAstc10x10SfloatBlock; } },
		{ hasher("Astc12x10SfloatBlock"), []() { return vk::Format::eAstc12x10SfloatBlock; } },
		{ hasher("Astc12x12SfloatBlock"), []() { return vk::Format::eAstc12x12SfloatBlock; } },
		{ hasher("Pvrtc12BppUnormBlockIMG"), []() { return vk::Format::ePvrtc12BppUnormBlockIMG; } },
		{ hasher("Pvrtc14BppUnormBlockIMG"), []() { return vk::Format::ePvrtc14BppUnormBlockIMG; } },
		{ hasher("Pvrtc22BppUnormBlockIMG"), []() { return vk::Format::ePvrtc22BppUnormBlockIMG; } },
		{ hasher("Pvrtc24BppUnormBlockIMG"), []() { return vk::Format::ePvrtc24BppUnormBlockIMG; } },
		{ hasher("Pvrtc12BppSrgbBlockIMG"), []() { return vk::Format::ePvrtc12BppSrgbBlockIMG; } },
		{ hasher("Pvrtc14BppSrgbBlockIMG"), []() { return vk::Format::ePvrtc14BppSrgbBlockIMG; } },
		{ hasher("Pvrtc22BppSrgbBlockIMG"), []() { return vk::Format::ePvrtc22BppSrgbBlockIMG; } },
		{ hasher("Pvrtc24BppSrgbBlockIMG"), []() { return vk::Format::ePvrtc24BppSrgbBlockIMG; } },
		{ hasher("A4B4G4R4UnormPack16EXT"), []() { return vk::Format::eA4B4G4R4UnormPack16EXT; } },
		{ hasher("A4R4G4B4UnormPack16EXT"), []() { return vk::Format::eA4R4G4B4UnormPack16EXT; } },
		{ hasher("Astc10x10SfloatBlockEXT"), []() { return vk::Format::eAstc10x10SfloatBlockEXT; } },
		{ hasher("Astc10x5SfloatBlockEXT"), []() { return vk::Format::eAstc10x5SfloatBlockEXT; } },
		{ hasher("Astc10x6SfloatBlockEXT"), []() { return vk::Format::eAstc10x6SfloatBlockEXT; } },
		{ hasher("Astc10x8SfloatBlockEXT"), []() { return vk::Format::eAstc10x8SfloatBlockEXT; } },
		{ hasher("Astc12x10SfloatBlockEXT"), []() { return vk::Format::eAstc12x10SfloatBlockEXT; } },
		{ hasher("Astc12x12SfloatBlockEXT"), []() { return vk::Format::eAstc12x12SfloatBlockEXT; } },
		{ hasher("Astc4x4SfloatBlockEXT"), []() { return vk::Format::eAstc4x4SfloatBlockEXT; } },
		{ hasher("Astc5x4SfloatBlockEXT"), []() { return vk::Format::eAstc5x4SfloatBlockEXT; } },
		{ hasher("Astc5x5SfloatBlockEXT"), []() { return vk::Format::eAstc5x5SfloatBlockEXT; } },
		{ hasher("Astc6x5SfloatBlockEXT"), []() { return vk::Format::eAstc6x5SfloatBlockEXT; } },
		{ hasher("Astc6x6SfloatBlockEXT"), []() { return vk::Format::eAstc6x6SfloatBlockEXT; } },
		{ hasher("Astc8x5SfloatBlockEXT"), []() { return vk::Format::eAstc8x5SfloatBlockEXT; } },
		{ hasher("Astc8x6SfloatBlockEXT"), []() { return vk::Format::eAstc8x6SfloatBlockEXT; } },
		{ hasher("Astc8x8SfloatBlockEXT"), []() { return vk::Format::eAstc8x8SfloatBlockEXT; } },
		{ hasher("B10X6G10X6R10X6G10X6422Unorm4Pack16KHR"), []() { return vk::Format::eB10X6G10X6R10X6G10X6422Unorm4Pack16KHR; } },
		{ hasher("B12X4G12X4R12X4G12X4422Unorm4Pack16KHR"), []() { return vk::Format::eB12X4G12X4R12X4G12X4422Unorm4Pack16KHR; } },
		{ hasher("B16G16R16G16422UnormKHR"), []() { return vk::Format::eB16G16R16G16422UnormKHR; } },
		{ hasher("B8G8R8G8422UnormKHR"), []() { return vk::Format::eB8G8R8G8422UnormKHR; } },
		{ hasher("G10X6B10X6G10X6R10X6422Unorm4Pack16KHR"), []() { return vk::Format::eG10X6B10X6G10X6R10X6422Unorm4Pack16KHR; } },
		{ hasher("G10X6B10X6R10X62Plane420Unorm3Pack16KHR"), []() { return vk::Format::eG10X6B10X6R10X62Plane420Unorm3Pack16KHR; } },
		{ hasher("G10X6B10X6R10X62Plane422Unorm3Pack16KHR"), []() { return vk::Format::eG10X6B10X6R10X62Plane422Unorm3Pack16KHR; } },
		{ hasher("G10X6B10X6R10X62Plane444Unorm3Pack16EXT"), []() { return vk::Format::eG10X6B10X6R10X62Plane444Unorm3Pack16EXT; } },
		{ hasher("G10X6B10X6R10X63Plane420Unorm3Pack16KHR"), []() { return vk::Format::eG10X6B10X6R10X63Plane420Unorm3Pack16KHR; } },
		{ hasher("G10X6B10X6R10X63Plane422Unorm3Pack16KHR"), []() { return vk::Format::eG10X6B10X6R10X63Plane422Unorm3Pack16KHR; } },
		{ hasher("G10X6B10X6R10X63Plane444Unorm3Pack16KHR"), []() { return vk::Format::eG10X6B10X6R10X63Plane444Unorm3Pack16KHR; } },
		{ hasher("G12X4B12X4G12X4R12X4422Unorm4Pack16KHR"), []() { return vk::Format::eG12X4B12X4G12X4R12X4422Unorm4Pack16KHR; } },
		{ hasher("G12X4B12X4R12X42Plane420Unorm3Pack16KHR"), []() { return vk::Format::eG12X4B12X4R12X42Plane420Unorm3Pack16KHR; } },
		{ hasher("G12X4B12X4R12X42Plane422Unorm3Pack16KHR"), []() { return vk::Format::eG12X4B12X4R12X42Plane422Unorm3Pack16KHR; } },
		{ hasher("G12X4B12X4R12X42Plane444Unorm3Pack16EXT"), []() { return vk::Format::eG12X4B12X4R12X42Plane444Unorm3Pack16EXT; } },
		{ hasher("G12X4B12X4R12X43Plane420Unorm3Pack16KHR"), []() { return vk::Format::eG12X4B12X4R12X43Plane420Unorm3Pack16KHR; } },
		{ hasher("G12X4B12X4R12X43Plane422Unorm3Pack16KHR"), []() { return vk::Format::eG12X4B12X4R12X43Plane422Unorm3Pack16KHR; } },
		{ hasher("G12X4B12X4R12X43Plane444Unorm3Pack16KHR"), []() { return vk::Format::eG12X4B12X4R12X43Plane444Unorm3Pack16KHR; } },
		{ hasher("G16B16G16R16422UnormKHR"), []() { return vk::Format::eG16B16G16R16422UnormKHR; } },
		{ hasher("G16B16R162Plane420UnormKHR"), []() { return vk::Format::eG16B16R162Plane420UnormKHR; } },
		{ hasher("G16B16R162Plane422UnormKHR"), []() { return vk::Format::eG16B16R162Plane422UnormKHR; } },
		{ hasher("G16B16R162Plane444UnormEXT"), []() { return vk::Format::eG16B16R162Plane444UnormEXT; } },
		{ hasher("G16B16R163Plane420UnormKHR"), []() { return vk::Format::eG16B16R163Plane420UnormKHR; } },
		{ hasher("G16B16R163Plane422UnormKHR"), []() { return vk::Format::eG16B16R163Plane422UnormKHR; } },
		{ hasher("G16B16R163Plane444UnormKHR"), []() { return vk::Format::eG16B16R163Plane444UnormKHR; } },
		{ hasher("G8B8G8R8422UnormKHR"), []() { return vk::Format::eG8B8G8R8422UnormKHR; } },
		{ hasher("G8B8R82Plane420UnormKHR"), []() { return vk::Format::eG8B8R82Plane420UnormKHR; } },
		{ hasher("G8B8R82Plane422UnormKHR"), []() { return vk::Format::eG8B8R82Plane422UnormKHR; } },
		{ hasher("G8B8R82Plane444UnormEXT"), []() { return vk::Format::eG8B8R82Plane444UnormEXT; } },
		{ hasher("G8B8R83Plane420UnormKHR"), []() { return vk::Format::eG8B8R83Plane420UnormKHR; } },
		{ hasher("G8B8R83Plane422UnormKHR"), []() { return vk::Format::eG8B8R83Plane422UnormKHR; } },
		{ hasher("G8B8R83Plane444UnormKHR"), []() { return vk::Format::eG8B8R83Plane444UnormKHR; } },
		{ hasher("R10X6G10X6B10X6A10X6Unorm4Pack16KHR"), []() { return vk::Format::eR10X6G10X6B10X6A10X6Unorm4Pack16KHR; } },
		{ hasher("R10X6G10X6Unorm2Pack16KHR"), []() { return vk::Format::eR10X6G10X6Unorm2Pack16KHR; } },
		{ hasher("R10X6UnormPack16KHR"), []() { return vk::Format::eR10X6UnormPack16KHR; } },
		{ hasher("R12X4G12X4B12X4A12X4Unorm4Pack16KHR"), []() { return vk::Format::eR12X4G12X4B12X4A12X4Unorm4Pack16KHR; } },
		{ hasher("R12X4G12X4Unorm2Pack16KHR"), []() { return vk::Format::eR12X4G12X4Unorm2Pack16KHR; } },
		{ hasher("R12X4UnormPack16KHR"), []() { return vk::Format::eR12X4UnormPack16KHR; } },
	};

	auto found = mFormatMap.find(formatHash);
	if (found != mFormatMap.end())
		return found->second();
	else
		return vk::Format::eUndefined;
}

vk::ImageUsageFlags CRenderSystemParser::parseUsage(const std::string& srUsage)
{
	vk::ImageUsageFlags usage;

	static std::map<size_t, vk::ImageUsageFlagBits> mUsageMap
	{
		{ hasher("TransferSrc"), vk::ImageUsageFlagBits::eTransferSrc},
		{ hasher("TransferDst"), vk::ImageUsageFlagBits::eTransferDst},
		{ hasher("Sampled"), vk::ImageUsageFlagBits::eSampled},
		{ hasher("Storage"), vk::ImageUsageFlagBits::eStorage},
		{ hasher("ColorAttachment"), vk::ImageUsageFlagBits::eColorAttachment},
		{ hasher("DepthStencilAttachment"), vk::ImageUsageFlagBits::eDepthStencilAttachment},
		{ hasher("TransientAttachment"), vk::ImageUsageFlagBits::eTransientAttachment},
		{ hasher("InputAttachment"), vk::ImageUsageFlagBits::eInputAttachment},
		#if defined( VK_ENABLE_BETA_EXTENSIONS )
		{ hasher("VideoDecodeDstKHR"), vk::ImageUsageFlagBits::eVideoDecodeDstKHR},
		{ hasher("VideoDecodeSrcKHR"), vk::ImageUsageFlagBits::eVideoDecodeSrcKHR},
		{ hasher("VideoDecodeDpbKHR"), vk::ImageUsageFlagBits::eVideoDecodeDpbKHR},
		#endif
		{ hasher("FragmentDensityMapEXT"), vk::ImageUsageFlagBits::eFragmentDensityMapEXT},
		{ hasher("FragmentShadingRateAttachmentKHR"), vk::ImageUsageFlagBits::eFragmentShadingRateAttachmentKHR},
		#if defined( VK_ENABLE_BETA_EXTENSIONS )
		{ hasher("VideoEncodeDstKHR"), vk::ImageUsageFlagBits::eVideoEncodeDstKHR},
		{ hasher("VideoEncodeSrcKHR"), vk::ImageUsageFlagBits::eVideoEncodeSrcKHR},
		{ hasher("VideoEncodeDpbKHR"), vk::ImageUsageFlagBits::eVideoEncodeDpbKHR},
		#endif /*VK_ENABLE_BETA_EXTENSIONS*/
		{ hasher("AttachmentFeedbackLoopEXT"), vk::ImageUsageFlagBits::eAttachmentFeedbackLoopEXT},
		{ hasher("InvocationMaskHUAWEI"), vk::ImageUsageFlagBits::eInvocationMaskHUAWEI},
		{ hasher("SampleWeightQCOM"), vk::ImageUsageFlagBits::eSampleWeightQCOM},
		{ hasher("SampleBlockMatchQCOM"), vk::ImageUsageFlagBits::eSampleBlockMatchQCOM},
		{ hasher("ShadingRateImageNV"), vk::ImageUsageFlagBits::eShadingRateImageNV},
	};

	parseFlags(srUsage, mUsageMap, usage);

	return usage;
}

std::vector<FCIDependency> CRenderSystemParser::parseDependencies(const std::vector<FJSONDependency>& dependencies)
{
	std::vector<FCIDependency> dependenciesCI;

	for (auto& dependency : dependencies)
	{
		FCIDependency dependencyCI;
		dependencyCI.src = parseDependencyDesc(dependency.src);
		dependencyCI.dst = parseDependencyDesc(dependency.dst);
		dependencyCI.depFlags = parseDependencyFlags(dependency.srDepFlags);
		dependenciesCI.emplace_back(dependencyCI);
	}

	return dependenciesCI;
}

FCIDependencyDesc CRenderSystemParser::parseDependencyDesc(const FJSONDependencyDesc& dependencyDesc)
{
	FCIDependencyDesc dependencyDescCI;
	dependencyDescCI.subpass = parseSubpass(dependencyDesc.srSubpass);
	dependencyDescCI.stageMask = parseStageMask(dependencyDesc.srStageMask);
	dependencyDescCI.accessMask = parseAccessMask(dependencyDesc.srAccessMask);
	return dependencyDescCI;
}

uint32_t CRenderSystemParser::parseSubpass(const std::string& subpass)
{
	auto subpassHash = hasher(subpass);

	if (is_digit(subpass))
		return std::stoul(subpass);
	else if (subpassHash == hasher("__EXTERNAL__"))
		return VK_SUBPASS_EXTERNAL;
}

vk::PipelineStageFlags CRenderSystemParser::parseStageMask(const std::string& stageMask)
{
	vk::PipelineStageFlags stageFlags;
	static std::map<size_t, vk::PipelineStageFlagBits> mStageMap
	{
		{ hasher("TopOfPipe"), vk::PipelineStageFlagBits::eTopOfPipe },
		{ hasher("DrawIndirect"), vk::PipelineStageFlagBits::eDrawIndirect },
		{ hasher("VertexInput"), vk::PipelineStageFlagBits::eVertexInput },
		{ hasher("VertexShader"), vk::PipelineStageFlagBits::eVertexShader },
		{ hasher("TessellationControlShader"), vk::PipelineStageFlagBits::eTessellationControlShader },
		{ hasher("TessellationEvaluationShader"), vk::PipelineStageFlagBits::eTessellationEvaluationShader },
		{ hasher("GeometryShader"), vk::PipelineStageFlagBits::eGeometryShader },
		{ hasher("FragmentShader"), vk::PipelineStageFlagBits::eFragmentShader },
		{ hasher("EarlyFragmentTests"), vk::PipelineStageFlagBits::eEarlyFragmentTests },
		{ hasher("LateFragmentTests"), vk::PipelineStageFlagBits::eLateFragmentTests },
		{ hasher("ColorAttachmentOutput"), vk::PipelineStageFlagBits::eColorAttachmentOutput },
		{ hasher("ComputeShader"), vk::PipelineStageFlagBits::eComputeShader },
		{ hasher("Transfer"), vk::PipelineStageFlagBits::eTransfer },
		{ hasher("BottomOfPipe"), vk::PipelineStageFlagBits::eBottomOfPipe },
		{ hasher("Host"), vk::PipelineStageFlagBits::eHost },
		{ hasher("AllGraphics"), vk::PipelineStageFlagBits::eAllGraphics },
		{ hasher("AllCommands"), vk::PipelineStageFlagBits::eAllCommands },
		{ hasher("None"), vk::PipelineStageFlagBits::eNone },
		{ hasher("TransformFeedbackEXT"), vk::PipelineStageFlagBits::eTransformFeedbackEXT },
		{ hasher("ConditionalRenderingEXT"), vk::PipelineStageFlagBits::eConditionalRenderingEXT },
		{ hasher("AccelerationStructureBuildKHR"), vk::PipelineStageFlagBits::eAccelerationStructureBuildKHR },
		{ hasher("RayTracingShaderKHR"), vk::PipelineStageFlagBits::eRayTracingShaderKHR },
		{ hasher("TaskShaderNV"), vk::PipelineStageFlagBits::eTaskShaderNV },
		{ hasher("MeshShaderNV"), vk::PipelineStageFlagBits::eMeshShaderNV },
		{ hasher("FragmentDensityProcessEXT"), vk::PipelineStageFlagBits::eFragmentDensityProcessEXT },
		{ hasher("FragmentShadingRateAttachmentKHR"), vk::PipelineStageFlagBits::eFragmentShadingRateAttachmentKHR },
		{ hasher("CommandPreprocessNV"), vk::PipelineStageFlagBits::eCommandPreprocessNV },
		{ hasher("AccelerationStructureBuildNV"), vk::PipelineStageFlagBits::eAccelerationStructureBuildNV },
		{ hasher("NoneKHR"), vk::PipelineStageFlagBits::eNoneKHR },
		{ hasher("RayTracingShaderNV"), vk::PipelineStageFlagBits::eRayTracingShaderNV },
		{ hasher("ShadingRateImageNV"), vk::PipelineStageFlagBits::eShadingRateImageNV },
	};

	parseFlags(stageMask, mStageMap, stageFlags);

	return stageFlags;
}


vk::AccessFlags CRenderSystemParser::parseAccessMask(const std::string& accessMask)
{
	vk::AccessFlags accessFlags;
	static std::map<size_t, vk::AccessFlagBits> mAccessMap
	{
		{ hasher("IndirectCommandRead"), vk::AccessFlagBits::eIndirectCommandRead },
		{ hasher("IndexRead"), vk::AccessFlagBits::eIndexRead },
		{ hasher("VertexAttributeRead"), vk::AccessFlagBits::eVertexAttributeRead },
		{ hasher("UniformRead"), vk::AccessFlagBits::eUniformRead },
		{ hasher("InputAttachmentRead"), vk::AccessFlagBits::eInputAttachmentRead },
		{ hasher("ShaderRead"), vk::AccessFlagBits::eShaderRead },
		{ hasher("ShaderWrite"), vk::AccessFlagBits::eShaderWrite },
		{ hasher("ColorAttachmentRead"), vk::AccessFlagBits::eColorAttachmentRead },
		{ hasher("ColorAttachmentWrite"), vk::AccessFlagBits::eColorAttachmentWrite },
		{ hasher("DepthStencilAttachmentRead"), vk::AccessFlagBits::eDepthStencilAttachmentRead },
		{ hasher("DepthStencilAttachmentWrite"), vk::AccessFlagBits::eDepthStencilAttachmentWrite },
		{ hasher("TransferRead"), vk::AccessFlagBits::eTransferRead },
		{ hasher("TransferWrite"), vk::AccessFlagBits::eTransferWrite },
		{ hasher("HostRead"), vk::AccessFlagBits::eHostRead },
		{ hasher("HostWrite"), vk::AccessFlagBits::eHostWrite },
		{ hasher("MemoryRead"), vk::AccessFlagBits::eMemoryRead },
		{ hasher("MemoryWrite"), vk::AccessFlagBits::eMemoryWrite },
		{ hasher("None"), vk::AccessFlagBits::eNone },
		{ hasher("TransformFeedbackWriteEXT"), vk::AccessFlagBits::eTransformFeedbackWriteEXT },
		{ hasher("TransformFeedbackCounterReadEXT"), vk::AccessFlagBits::eTransformFeedbackCounterReadEXT },
		{ hasher("TransformFeedbackCounterWriteEXT"), vk::AccessFlagBits::eTransformFeedbackCounterWriteEXT },
		{ hasher("ConditionalRenderingReadEXT"), vk::AccessFlagBits::eConditionalRenderingReadEXT },
		{ hasher("ColorAttachmentReadNoncoherentEXT"), vk::AccessFlagBits::eColorAttachmentReadNoncoherentEXT },
		{ hasher("AccelerationStructureReadKHR"), vk::AccessFlagBits::eAccelerationStructureReadKHR },
		{ hasher("AccelerationStructureWriteKHR"), vk::AccessFlagBits::eAccelerationStructureWriteKHR },
		{ hasher("FragmentDensityMapReadEXT"), vk::AccessFlagBits::eFragmentDensityMapReadEXT },
		{ hasher("FragmentShadingRateAttachmentReadKHR"), vk::AccessFlagBits::eFragmentShadingRateAttachmentReadKHR },
		{ hasher("CommandPreprocessReadNV"), vk::AccessFlagBits::eCommandPreprocessReadNV },
		{ hasher("CommandPreprocessWriteNV"), vk::AccessFlagBits::eCommandPreprocessWriteNV },
		{ hasher("AccelerationStructureReadNV"), vk::AccessFlagBits::eAccelerationStructureReadNV },
		{ hasher("AccelerationStructureWriteNV"), vk::AccessFlagBits::eAccelerationStructureWriteNV },
		{ hasher("NoneKHR"), vk::AccessFlagBits::eNoneKHR },
		{ hasher("ShadingRateImageReadNV"), vk::AccessFlagBits::eShadingRateImageReadNV }
	};

	parseFlags(accessMask, mAccessMap, accessFlags);

	return accessFlags;
}

vk::DependencyFlags CRenderSystemParser::parseDependencyFlags(const std::string& flags)
{
	vk::DependencyFlags dependencyFlags;
	static std::map<size_t, vk::DependencyFlagBits> mDependencyMap
	{
		{ hasher("ByRegion"), vk::DependencyFlagBits::eByRegion },
		{ hasher("DeviceGroup"), vk::DependencyFlagBits::eDeviceGroup },
		{ hasher("ViewLocal"), vk::DependencyFlagBits::eViewLocal },
		{ hasher("FeedbackLoopEXT"), vk::DependencyFlagBits::eFeedbackLoopEXT },
		{ hasher("DeviceGroupKHR"), vk::DependencyFlagBits::eDeviceGroupKHR },
		{ hasher("ViewLocalKHR"), vk::DependencyFlagBits::eViewLocalKHR },
	};

	parseFlags(flags, mDependencyMap, dependencyFlags);

	return dependencyFlags;
}

std::vector<std::string> CRenderSystemParser::tokenize(const std::string& input, char delimeter)
{
	std::vector<std::string> tokens;
	std::string token;
	std::stringstream ss(input);

	while (std::getline(ss, token, delimeter))
	{
		token.erase(std::remove_if(token.begin(), token.end(), isspace), token.end());
		tokens.emplace_back(token);
	}

	return tokens;
}

bool CRenderSystemParser::is_digit(const std::string& input)
{
	return std::all_of(input.begin(), input.end(), ::isdigit);
}