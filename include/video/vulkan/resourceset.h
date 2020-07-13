#ifndef RESOURCESET_H
#define RESOURCESET_H

#include <vector>
#include <vulkan/vulkan.h>

namespace Boiler
{
	namespace Vulkan
	{
		struct ResourceSet
		{
			const ResourceId resourceId;
			
			ResourceSet(ResourceId id) : resourceId(id)
			{
			}

			std::vector<VkBuffer> buffers;
			std::vector<VkDeviceMemory> deviceMemory;
		};
	}
}

#endif /* RESOURCESET_H */
