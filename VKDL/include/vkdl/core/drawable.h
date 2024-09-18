#pragma once

#include "render_target.h"
#include "render_states.h"

VKDL_BEGIN

class Drawable
{
public:
	virtual ~Drawable() {};

	virtual void draw(RenderTarget& target, RenderStates& states, const RenderOptions& options) const = 0;
};

VKDL_END