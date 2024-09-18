#pragma once

#include "render_options.h"
#include "../graphics/color.h"

VKDL_BEGIN

class Drawable;
class RenderOptions;

class RenderTarget abstract
{
	friend class Drawable;

public:
	virtual void render(const Drawable& drawable, const RenderOptions& options = {}) = 0;
	virtual void display() = 0;
	virtual void clear(const Color& color = Colors::Black) = 0;

	virtual vk::CommandBuffer getCommandBuffer() = 0;
	virtual vk::Framebuffer getFrameBuffer() = 0;
	virtual uvec2 getFrameBufferSize() const = 0;
	virtual vk::ClearColorValue getClearColorValue() const = 0;
};

VKDL_END