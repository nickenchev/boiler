#ifndef DESCRIPTORSET_H
#define DESCRIPTORSET_H

#include <vector>
#include "core/logger.h"
#include "display/vulkan.h"

namespace Boiler { namespace Vulkan {

class DescriptorSet
{
	VkDescriptorSetLayout layout;
	VkDescriptorPool pool;
	unsigned int maxSets;
	std::vector<VkDescriptorSet> sets;
	
public:
	DescriptorSet()
	{
		layout = VK_NULL_HANDLE;
		pool = VK_NULL_HANDLE;
	}

	void setMaxSets(unsigned int maxSets)
	{
		sets.resize(maxSets);
		this->maxSets = maxSets;
	}
	unsigned int getMaxSets() const { return maxSets; }

	const VkDescriptorSetLayout &getLayout() const { return layout; }

	VkDescriptorSet getSet(uint32_t descriptorIndex) const
	{
		assert(descriptorIndex < sets.size());
		return sets[descriptorIndex];
	}

	template<unsigned int Size>
	void createLayout(VkDevice device, const std::array<VkDescriptorSetLayoutBinding, Size> &bindings)
	{
		VkDescriptorSetLayoutCreateInfo layoutInfo = {};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = bindings.size();
		layoutInfo.pBindings = bindings.data();

		if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &layout) != VK_SUCCESS)
		{
			throw std::runtime_error("Error creating descriptor set layout");
		}
	}

	template<unsigned int Size>
	void createPool(VkDevice device, const std::array<VkDescriptorPoolSize, Size> &poolSizes)
	{
		VkDescriptorPoolCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		createInfo.poolSizeCount = poolSizes.size();
		createInfo.pPoolSizes = poolSizes.data();
		createInfo.maxSets = maxSets;

		if (vkCreateDescriptorPool(device, &createInfo, nullptr, &pool) != VK_SUCCESS)
		{
			throw std::runtime_error("Unable to create descriptor pool");
		}
	}

	void allocate(VkDevice device)
	{
		// need to copy layout into array of layouts since create call expects array
		std::vector<VkDescriptorSetLayout> layouts(maxSets, layout);

		VkDescriptorSetAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = pool;
		allocInfo.descriptorSetCount = maxSets;
		allocInfo.pSetLayouts = layouts.data();

		if (vkAllocateDescriptorSets(device, &allocInfo, sets.data()) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to allocate descriptor sets");
		}
	}

	void cleanup(VkDevice device)
	{
		vkDestroyDescriptorPool(device, pool, nullptr);
		vkDestroyDescriptorSetLayout(device, layout, nullptr);
	}
};

}};
#endif /* DESCRIPTORSET_H */
