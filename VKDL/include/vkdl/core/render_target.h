#pragma once

#include "include_vulkan.h"
#include "../math/vector_type.h"
#include "../graphics/color.h"

VKDL_BEGIN

class Drawable;
class RenderOptions;

class RenderTarget abstract
{
	friend class Drawable;

public:
	virtual void render(const Drawable& drawable, const RenderOptions& options) = 0;
	virtual void display() = 0;
	virtual void clear(const Color& color = Colors::Black) = 0;

	virtual vk::CommandBuffer getCommandBuffer() = 0;
	virtual vk::Framebuffer getFrameBuffer() = 0;
	virtual uvec2 getFrameBufferSize() const = 0;
};

VKDL_END