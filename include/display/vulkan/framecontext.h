#ifndef FRAMECONTEXT_H
#define FRAMECONTEXT_H

#include "display/vulkan.h"

namespace Boiler::Vulkan
{

class FrameContext
{
	VkCommandBuffer mainCommandBuffer, renderCommandBuffer, deferredCommandBuffer, skyboxCommandBuffer;
	VkDescriptorSet frameDescriptorSet;
public:
    FrameContext();
    virtual ~FrameContext();
};

}

#endif /* FRAMECONTEXT_H */
