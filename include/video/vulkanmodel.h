#ifndef VULKANMODEL_H
#define VULKANMODEL_H

#include "model.h"
#include "vertexdata.h"

namespace Boiler
{

class VulkanModel : public Model
{
public:
    VulkanModel(const VertexData &data);
    virtual ~VulkanModel();
};

}
#endif /* VULKANMODEL_H */
