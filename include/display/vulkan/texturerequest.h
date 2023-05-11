#ifndef TEXTUREREQUEST_H
#define TEXTUREREQUEST_H

#include "core/size.h"
#include "vulkan.h"

namespace Boiler { namespace Vulkan {

struct TextureRequest
{
	/*
	auto imagePair = createImage(Size(swapChainExtent.width, swapChainExtent.height), depthFormat,
								 VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
								 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	*/
	Size size;
	VkFormat format;
	VkImageTiling tiling;
	VkImageUsageFlags usage;
	VkMemoryPropertyFlags memProperties;
	int layers;
	VkImageCreateFlags flags;

	TextureRequest(const Size &size, VkFormat format)
	{
		this->format = format;
		this->size = size;
		this->layers = 1;
		this->tiling = VK_IMAGE_TILING_OPTIMAL;
		this->usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
		this->memProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		this->flags = 0;
	}

	VkImageCreateInfo createInfo() const
	{
		VkImageCreateInfo imageInfo = {};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = static_cast<uint32_t>(size.width);
		imageInfo.extent.height = static_cast<uint32_t>(size.height);
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = 1;
		imageInfo.arrayLayers = layers;
		imageInfo.format = format;
		imageInfo.tiling = tiling;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage = usage;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageInfo.flags = flags;

		return imageInfo;
	}
};

}}
#endif /* TEXTUREREQUEST_H */
