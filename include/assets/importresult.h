#ifndef IMPORTRESULT_H
#define IMPORTRESULT_H

#include <vector>
#include "display/material.h"
#include "display/mesh.h"
#include "animation/animation.h"
#include "animation/common.h"

namespace Boiler {

struct ImportResult
{
	std::vector<AssetId> materialsIds;
	std::vector<Mesh> meshes;
    std::vector<AnimationId> animations;
};

};

#endif /* IMPORTRESULT_H */
