#pragma once

#include <optional>
#include "../core/include_vulkan.h"
#include "../math/transform_2d.h"

VKDL_BEGIN

class RenderOptions
{
public:
	RenderOptions();
	RenderOptions(const Transform2D& transform);

	std::optional<Transform2D>  transform;
	std::optional<vk::Viewport> viewport;
	std::optional<vk::Rect2D>   scissor;
	bool                        lazyRendering;
};

VKDL_END