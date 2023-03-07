#include "QueueFamily.h"

using namespace engine::graphics;

void CQueueFamilies::initialize(const vk::PhysicalDevice& vkPhysical, vk::SurfaceKHR& vkSurface)
{
	// Temporary
	bool ignore{ false };

	mFamilyUsages.clear();
	vUniqueFamilies.clear();
	mQueueFamilies.clear();
	mSelectedFamilies.clear();

	auto queueFamilies = vkPhysical.getQueueFamilyProperties();

	// Saving families in map
	for (size_t index = 0; index < queueFamilies.size(); ++index)
	{
		mQueueFamilies[index] = queueFamilies[index];
		const auto& prop = queueFamilies[index];

		if (prop.queueCount == 0)
			continue;

		const bool has_graphics = static_cast<bool>(prop.queueFlags & vk::QueueFlagBits::eGraphics);
		const bool has_compute = static_cast<bool>(prop.queueFlags & vk::QueueFlagBits::eCompute);
		const bool has_transfer = static_cast<bool>(prop.queueFlags & vk::QueueFlagBits::eTransfer);

		// Looking for suitable graphics family
		if (has_graphics && !isFamilySelected(family::graphics))
			mSelectedFamilies[family::graphics] = index;

		// Looking for family that support graphics and compute
		if (has_graphics && has_compute && !isFamilySelected(family::graphics_compute))
			mSelectedFamilies[family::graphics_compute] = index;

		// Looking for family that support graphics and transfer
		if (has_graphics && has_transfer && !isFamilySelected(family::graphics_transfer))
			mSelectedFamilies[family::graphics_transfer] = index;

		if (!ignore)
		{
			// Looking for compute only family
			if (has_compute && !has_graphics && !isFamilySelected(family::compute))
				mSelectedFamilies[family::compute] = index;

			// Looking for transfer only family
			if (has_transfer && !has_graphics && !has_compute && !isFamilySelected(family::transfer))
				mSelectedFamilies[family::transfer] = index;
		}

		// Looking for present family
		if (vkPhysical.getSurfaceSupportKHR(index, vkSurface) && !isFamilySelected(family::present))
			mSelectedFamilies[family::present] = index;
	}

	// Check graphics family
	if (!isFamilySelected(family::graphics))
		log_error("No suitable queue families found for vk::QueueFlagBits::eGraphics.");

	// Check graphics + compute family
	if (!isFamilySelected(family::graphics_compute))
		log_error("No suitable queue families found for vk::QueueFlagBits::eGraphics and vk::QueueFlagBits::eCompute.");

	// Check graphics + transfer family
	if (!isFamilySelected(family::graphics_transfer))
		log_error("No suitable queue families found for vk::QueueFlagBits::eGraphics and vk::QueueFlagBits::eTransfer.");

	// Check present family
	if (!isFamilySelected(family::present))
		log_error("No suitable queue families found for present.");

	// Trying again to find compute and transfer families
	for (auto& [index, prop] : mQueueFamilies)
	{
		if (prop.queueCount == 0)
			continue;

		// Looking for compute only family
		if ((prop.queueFlags & vk::QueueFlagBits::eCompute) && !isFamilySelected(family::compute))
			mSelectedFamilies[family::compute] = index;
		// Looking for transfer only family
		if ((prop.queueFlags & vk::QueueFlagBits::eTransfer) && !isFamilySelected(family::transfer))
			mSelectedFamilies[family::transfer] = index;
	}

	// Check for compute family
	if (!isFamilySelected(family::compute))
		log_error("No suitable queue families found for vk::QueueFlagBits::eCompute.");

	// Check for transfer family
	if (!isFamilySelected(family::transfer))
		log_error("No suitable queue families found for vk::QueueFlagBits::eTransfer.");

	// Counting family usages and unique families
	std::set<uint32_t> uniqueFamilies;
	for (auto& [family, index] : mSelectedFamilies)
	{
		mFamilyUsages[index]++;
		uniqueFamilies.emplace(index);
	}

	vUniqueFamilies = std::vector<uint32_t>(uniqueFamilies.begin(), uniqueFamilies.end());
}

std::vector<vk::DeviceQueueCreateInfo> CQueueFamilies::getCreateInfos()
{
	std::vector<vk::DeviceQueueCreateInfo> createInfos;

	for (auto& [index, usages] : mFamilyUsages)
	{
		// Allocating temporary pointer for queue priorities
		for (size_t idx = 0; idx < usages; ++idx)
			mPriorities[index].emplace_back(1.f);

		// Making queue create info
		vk::DeviceQueueCreateInfo createInfo;
		createInfo.flags = vk::DeviceQueueCreateFlags{};
		createInfo.queueFamilyIndex = index;
		createInfo.queueCount = usages;
		createInfo.pQueuePriorities = mPriorities[index].data();

		createInfos.emplace_back(createInfo);
	}

	return createInfos;
}

void CQueueFamilies::clearCreateInfo()
{
	mPriorities.clear();
}

vk::Queue CQueueFamilies::createQueue(uint32_t type, vk::Device& vkDevice)
{
	// Count queue usages
	static std::unordered_map<uint32_t, uint32_t> queueUsages;
	
	auto familyIndex = getFamilyIndex(type);
	return vkDevice.getQueue(familyIndex, queueUsages[familyIndex]++);
}

const uint32_t CQueueFamilies::getFamilyIndex(uint32_t type) const
{
	auto it = mSelectedFamilies.find(type);
	if(it != mSelectedFamilies.end())
		return it->second;

	return ~0u;
}

const std::vector<uint32_t>& CQueueFamilies::getUniqueFamilies() const
{
	return vUniqueFamilies;
}

bool CQueueFamilies::isValid()
{
	// Check is all queue families found and that is enought queues in family
	bool isAllFamiliesFound{ true };
	bool isEnoughQueuesInFamily{ true };

	for (auto& [type, family] : mSelectedFamilies)
	{
		isAllFamiliesFound = isAllFamiliesFound && isFamilySelected(type);
		isEnoughQueuesInFamily = isEnoughQueuesInFamily && (mQueueFamilies[family].queueCount >= mFamilyUsages[family]);
	}

	return isAllFamiliesFound && isEnoughQueuesInFamily;
		
}

bool CQueueFamilies::isFamilySelected(uint32_t type)
{
	return mSelectedFamilies.count(type) != 0;
}