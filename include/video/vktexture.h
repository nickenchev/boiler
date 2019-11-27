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
	const VkImageView imageView;

public:
    VkTexture(const std::string &filePath, const VkDevice &device, const VkImage image,
			  const VkDeviceMemory memory, const VkImageView imageView)
		: device(device), image(image), memory(memory), imageView(imageView), Texture(filePath)
	{
	}

	~VkTexture()
	{
		vkDestroyImageView(device, imageView, nullptr);
		vkFreeMemory(device, memory, nullptr);
		vkDestroyImage(device, image, nullptr);
	}

	const VkImageView &getImageView() const { return imageView; }
};

}

#endif /* VKTEXTURE_H */
