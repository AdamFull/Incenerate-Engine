#include "EffectShared.h"

#include "Engine.h"
#include "graphics/image/Image2D.h"

using namespace engine::graphics;
using namespace engine::ecs;

size_t effectshared::createImage(const std::string& name, vk::Format format, bool mips, uint32_t mip_levels)
{
	using namespace engine;

	auto& graphics = EGEngine->getGraphics();
	auto& device = graphics->getDevice();

	auto pImage = std::make_unique<CImage2D>(device.get());
	pImage->create(
		device->getExtent(true),
		format,
		vk::ImageLayout::eGeneral,
		vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eStorage,
		vk::ImageAspectFlagBits::eColor,
		vk::Filter::eLinear,
		vk::SamplerAddressMode::eClampToEdge,
		vk::SampleCountFlagBits::e1, true, false, mips, mip_levels);

	return graphics->addImage(name, std::move(pImage));
}

void effectshared::tryReCreateImage(const std::string& name, size_t& image_id, vk::Format format, bool mips, uint32_t mip_levels)
{
	using namespace engine;

	auto& graphics = EGEngine->getGraphics();
	auto& device = graphics->getDevice();
	auto extent = device->getExtent(true);

	auto& image = graphics->getImage(image_id);
	auto image_ext = image->getExtent();
	if (image_ext.width != extent.width || image_ext.height != extent.height)
	{
		graphics->removeImage(image_id);
		image_id = createImage(name, format, mips, mip_levels);
	}
}