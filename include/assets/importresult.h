#ifndef IMPORTRESULT_H
#define IMPORTRESULT_H

#include <vector>
#include "animation/animation.h"

namespace Boiler {

struct ImportResult
{
    std::vector<Entity> entities;
    std::vector<AnimationId> animations;
};

};

#endif /* IMPORTRESULT_H */
