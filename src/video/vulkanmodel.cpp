#include <stdexcept>
#include "video/renderer.h"
#include "video/vulkanmodel.h"
#include "core/logger.h"

using namespace Boiler;

VulkanModel::VulkanModel(unsigned int vertexCount, unsigned int indexCount, ResourceId resourceId)
	: Model(vertexCount, indexCount), resourceId(resourceId)
{
}
