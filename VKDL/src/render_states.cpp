#include "../include/vkdl/core/render_states.h"

#include "../include/vkdl/core/drawable.h"
#include "../include/vkdl/core/render_target.h"
#include "../include/vkdl/core/render_options.h"
#include "../include/vkdl/core/context.h"

template <class T>
static bool check_update(T& val, const std::optional<T>& new_val)
{
	if (!new_val.has_value() || val == *new_val) return false;
	val = *new_val;
	return true;
}

template <class T>
static bool check_update(T& val, const T& new_val)
{
	if (val == new_val) return false;
	val = new_val;
	return true;
}

VKDL_BEGIN

void RenderStates::updateRenderPassUUID(const UUID& uuid)
{
	renderpass_uuid.update_value(uuid);
}

void RenderStates::updatePipelineUUID(const UUID& uuid)
{
	pipeline_uuid.update_value(uuid);
}

void RenderStates::updateViewport(const vk::Viewport& viewport)
{
	this->viewport.update_value(viewport);
}

void RenderStates::updateScissor(vk::Rect2D scissor)
{
	this->scissor.update_value(scissor);
}

void RenderStates::reset(const RenderTarget& target)
{
	auto fb_size = target.getFrameBufferSize();

	renderpass_uuid.reset();
	pipeline_uuid.reset();
	viewport.reset({ 0.f, 0.f, (float)fb_size.x, (float)fb_size.y, 0.f, 0.f });
	scissor.reset({ { 0, 0 }, { fb_size.x, fb_size.y } });
}

void RenderStates::bind(RenderTarget& target, const RenderOptions& options)
{
	auto& ctx = Context::get();
	auto cmd  = target.getCommandBuffer();

	if (renderpass_uuid.check_and_update()) {
		auto fb_size = target.getFrameBufferSize();

		vk::ClearValue clear_value = {};
		clear_value.color = target.getClearColorValue();

		vk::RenderPassBeginInfo image_info = {};
		image_info.renderPass      = ctx.getRenderpass(renderpass_uuid.value);
		image_info.framebuffer     = target.getFrameBuffer();
		image_info.renderArea      = vk::Rect2D({ 0, 0 }, { fb_size.x, fb_size.y });
		image_info.clearValueCount = 1;
		image_info.pClearValues    = &clear_value;

		cmd.beginRenderPass(image_info, vk::SubpassContents::eInline);
	}

	if (pipeline_uuid.check_and_update()) {
		cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, ctx.getPipeline(pipeline_uuid.value));
	}

	if (viewport.check_and_update(options.viewport)) {
		cmd.setViewport(0, 1, &viewport.value);
	}

	if (scissor.check_and_update(options.scissor)) {
		cmd.setScissor(0, 1, &scissor.value);
	}
}

VKDL_END
