#pragma once

#include "config.h"
#include "../util/uuid.h"

VKDL_BEGIN

class RenderTarget;
class RenderStates;
class RenderOptions;

class Drawable
{
public:
	virtual void draw(RenderTarget& target, RenderStates& states, const RenderOptions& options) const = 0;
};

VKDL_END