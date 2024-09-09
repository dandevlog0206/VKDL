#include "../include/vkdl/core/render_options.h"

VKDL_BEGIN

RenderOptions::RenderOptions() :
	lazyRendering(false)
{
}

RenderOptions::RenderOptions(const Transform2D& transform) :
	lazyRendering(false),
	transform(transform)
{
}

VKDL_END