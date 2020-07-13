#ifndef VULKANMODEL_H
#define VULKANMODEL_H

#include "model.h"
#include "vertexdata.h"
#include "video/renderer.h"
#include "video/vulkan.h"

namespace Boiler
{

class VulkanModel : public Model
{
	const ResourceId resourceId;

public:
	VulkanModel(unsigned int vertexCount, unsigned int indexCount, ResourceId resourceId);

	ResourceId getResourceId() const { return resourceId; }
};

}
#endif /* VULKANMODEL_H */
