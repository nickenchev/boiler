#ifndef SAMPLER_H
#define SAMPLER_H

#include <stdexcept>
#include "video/vulkan.h"

constexpr int maxAnistrophy = 16;

namespace Boiler { namespace Vulkan {

class Sampler
{
	VkSampler sampler;
	
public:
	Sampler()
	{
		sampler = VK_NULL_HANDLE;
	}

	Sampler(VkSampler sampler)
	{
		this->sampler = sampler;
	}

	static Sampler create(VkDevice device, VkSamplerAddressMode addressMode)
	{
		VkSamplerCreateInfo samplerInfo = {};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = VK_FILTER_LINEAR;
		samplerInfo.minFilter = VK_FILTER_LINEAR;
		samplerInfo.addressModeU = addressMode;
		samplerInfo.addressModeV = addressMode;
		samplerInfo.addressModeW = addressMode;
		samplerInfo.anisotropyEnable = VK_TRUE;
		samplerInfo.maxAnisotropy = maxAnistrophy;
		samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		samplerInfo.unnormalizedCoordinates = VK_FALSE;
		samplerInfo.compareEnable = VK_FALSE;
		samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerInfo.mipLodBias = 0.0f;
		samplerInfo.minLod = 0.0f;
		samplerInfo.maxLod = 0.0f;

		VkSampler textureSampler = VK_NULL_HANDLE;
		if (vkCreateSampler(device, &samplerInfo, nullptr, &textureSampler) != VK_SUCCESS)
		{
			throw std::runtime_error("Error creating sampler");
		}

		return Sampler(textureSampler);
	}

	void destroy(VkDevice device)
	{
		if (sampler != VK_NULL_HANDLE)
		{
			vkDestroySampler(device, sampler, nullptr);
		}
	}

	VkSampler vkSampler() const { return sampler; }
};

}}
#endif /* SAMPLER_H */
