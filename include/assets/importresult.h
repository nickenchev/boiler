#ifndef IMPORTRESULT_H
#define IMPORTRESULT_H

#include <vector>
#include "video/material.h"
#include "video/mesh.h"
#include "animation/animation.h"
#include "animation/common.h"

namespace Boiler {

struct ImportResult
{
	std::vector<Material> materials;
	std::vector<Mesh> meshes;
    std::vector<AnimationId> animations;
};

};

#endif /* IMPORTRESULT_H */
