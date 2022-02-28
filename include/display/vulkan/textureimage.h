#ifndef TEXTUREIMAGE_H
#define TEXTUREIMAGE_H

#include "display/vulkan.h"

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
};

}
#endif /* TEXTUREIMAGE_H */
