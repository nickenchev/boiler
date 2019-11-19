#ifndef MODELVIEWPROJECTION_H
#define MODELVIEWPROJECTION_H

#include "glm/glm.hpp"

namespace Boiler
{

struct ModelViewProjection
{
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 projection;
};

}

#endif /* MODELVIEWPROJECTION_H */
