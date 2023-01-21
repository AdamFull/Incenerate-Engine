#include "EffectShared.h"

#include "Engine.h"
#include "graphics/image/Image2D.h"

using namespace engine::graphics;
using namespace engine::ecs;

size_t effectshared::createImage(const std::string& name, vk::Format format, bool mips)
{
	using namespace engine;

	auto& device = EGGraphics->getDevice();

	auto pImage = std::make_unique<CImage2D>(device.get());
	pImage->create(
		device->getExtent(true),
		format,
		vk::ImageLayout::eGeneral,
		vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst |
		vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eColorAttachment |
		vk::ImageUsageFlagBits::eStorage,
		vk::ImageAspectFlagBits::eColor,
		vk::Filter::eLinear,
		vk::SamplerAddressMode::eClampToEdge,
		vk::SampleCountFlagBits::e1, true, false, mips, 5);

	return EGGraphics->addImage(name, std::move(pImage));
}

void effectshared::tryReCreateImage(const std::string& name, size_t& image_id, vk::Format format, bool mips)
{
	using namespace engine;

	auto& device = EGGraphics->getDevice();
	auto extent = device->getExtent(true);

	auto& image = EGGraphics->getImage(image_id);
	auto image_ext = image->getExtent();
	if (image_ext.width != extent.width || image_ext.height != extent.height)
	{
		EGGraphics->removeImage(image_id);
		image_id = createImage(name, format, mips);
	}
}