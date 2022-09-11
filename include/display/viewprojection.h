#ifndef MODELVIEWPROJECTION_H
#define MODELVIEWPROJECTION_H
#include "glm/glm.hpp"

namespace Boiler
{

struct ViewProjection
{
	glm::mat4 view;
	glm::mat4 projection;
	glm::mat4 orthographic;
};

}

#endif /* MODELVIEWPROJECTION_H */
