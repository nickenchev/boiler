#ifndef TEXTUREIMAGE_H
#define TEXTUREIMAGE_H

#include "vulkan.h"

namespace Boiler {

struct TextureImage
{
	VkImage image;
	VkImageView imageView;
	VkDeviceMemory memory;

	TextureImage()
	{
		image = VK_NULL_HANDLE;
		imageView = VK_NULL_HANDLE;
		memory = VK_NULL_HANDLE;
	}

	TextureImage(VkImage image, VkDeviceMemory memory, VkImageView imageView)
	{
		this->image = image;
		this->imageView = imageView;
		this->memory = memory;
	}
};

}
#endif /* TEXTUREIMAGE_H */
