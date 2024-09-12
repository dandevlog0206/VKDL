#pragma once

#include "../math/rect.h"

VKDL_BEGIN

struct Glyph
{
    float advance;
    int   lsb_delta;
    int   rsb_delta;
    rect  bounds;
    irect texture_rect;
};

VKDL_END