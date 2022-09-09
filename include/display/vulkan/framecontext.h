#ifndef FRAMECONTEXT_H
#define FRAMECONTEXT_H

#include "display/vulkan.h"

namespace Boiler::Vulkan
{

class FrameContext
{
	VkDescriptorSet frameDescriptorSet;
	VkDescriptorSet materialDescriptorSet;
public:
    FrameContext();
    virtual ~FrameContext();
};

}

#endif /* FRAMECONTEXT_H */
