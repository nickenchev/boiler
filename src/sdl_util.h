#ifndef SDL_UTIL_H
#define SDL_UTIL_H

#include <SDL.h>
#include "rect.h"

SDL_Rect make_rect(glm::vec2 position, Size size)
{
    SDL_Rect rect;
    rect.x = position.x;
    rect.y = position.y;
    rect.w = size.width;
    rect.h = size.height;
    return rect;
}

SDL_Rect make_rect(Rect frame)
{
    return make_rect(frame.position, frame.size);
}

#endif /* SDL_UTIL_H */
