#ifndef VKTEXTURE_H
#define VKTEXTURE_H

#include "texture.h"
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

namespace Boiler
{

class VkTexture : public Texture
{
	const VkDevice &device;
	const VkImage image;
	const VkDeviceMemory memory;

public:
    VkTexture(const std::string &filePath, const VkDevice &device, const VkImage image, const VkDeviceMemory memory)
		: device(device), image(image), memory(memory), Texture(filePath)
	{
	}

	~VkTexture()
	{
		vkFreeMemory(device, memory, nullptr);
		vkDestroyImage(device, image, nullptr);
	}
};

}

#endif /* VKTEXTURE_H */
