#ifndef RESOURCESET_H
#define RESOURCESET_H

#include <vector>
#include <vulkan/vulkan.h>
#include "core/asset.h"

namespace Boiler { namespace Vulkan
{

struct ResourceSet
{
	const AssetId assetId;

	ResourceSet(AssetId assetId) : assetId(assetId) { }

	std::vector<VkBuffer> buffers;
	std::vector<VkDeviceMemory> deviceMemory;
	std::vector<VkImage> images;
	std::vector<VkImageView> imageViews;
};

} }

#endif /* RESOURCESET_H */
