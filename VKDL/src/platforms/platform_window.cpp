#ifdef _WIN32
#  include "windows_platform_window_impl.h"
#endif

#include "../../include/vkdl/core/drawable.h"

VKDL_BEGIN

void PlatformWindow::render(const Drawable& drawable, const RenderOptions& options)
{
	if (impl->render_begin == false) {
		auto& device = Context::get().device;
		
		impl->acquireSwapchainImage();

		auto cmd = getCommandBuffer();
		cmd.begin({ vk::CommandBufferUsageFlagBits::eOneTimeSubmit});

		auto& frame = impl->frames[impl->frame_idx];

		VK_CHECK(device.waitForFences(1, &frame.fence, true, UINT64_MAX));
		VK_CHECK(device.resetFences(1, &frame.fence));

		frame.states.reset(*this);

		impl->render_begin = true;
	}

	drawable.draw(*this, impl->frames[impl->frame_idx].states, options);
}

void PlatformWindow::display()
{
	auto& ctx    = Context::get();
	auto& device = ctx.device;
	auto& frame  = impl->frames[impl->frame_idx];
	
	auto& cmd             = frame.cmd_buffer;
	auto& semaphores      = impl->semaphores[impl->semaphore_idx];
	auto& image_acquired  = semaphores.image_acquired;
	auto& render_complete = semaphores.render_complete;

	cmd.endRenderPass();
	cmd.end();

	vk::PipelineStageFlags wait_stage = vk::PipelineStageFlagBits::eColorAttachmentOutput;

	vk::SubmitInfo submit_info = {
		1, &image_acquired,
		&wait_stage,
		1, &cmd,
		1, &render_complete
	};

	VK_CHECK(ctx.queues[ctx.graphics_queue_family_idx].submit(1, &submit_info, frame.fence));

	vk::PresentInfoKHR present_info = {
		1, &render_complete,
		1, &impl->swapchain,
		&impl->frame_idx,
	};

	VK_CHECK(ctx.queues[impl->present_queue_family_idx].presentKHR(&present_info));

	impl->semaphore_idx = (impl->semaphore_idx + 1) % impl->semaphores.size();
	impl->render_begin = false;
}

void PlatformWindow::clear(const Color& color)
{
	//auto& frame = impl->frames[impl->frame_idx];
	//auto& cmd   = frame.cmd_buffer;

	//vk::ClearColorValue clear_color = {};
	//clear_color.uint32[0] = color.r;
	//clear_color.uint32[1] = color.g;
	//clear_color.uint32[2] = color.b;
	//clear_color.uint32[3] = color.a;

	//vk::ImageSubresourceRange range = {};
	//range.aspectMask     = vk::ImageAspectFlagBits::eColor;
	//range.baseMipLevel   = 0;
	//range.levelCount     = 1;
	//range.baseArrayLayer = 0;
	//range.layerCount     = 1;

	//frame.states.bind(*this, {});

	//cmd.clearColorImage(
	//	frame.image,
	//	vk::ImageLayout::eTransferDstOptimal,
	//	&clear_color,
	//	1, &range);
}

vk::CommandBuffer PlatformWindow::getCommandBuffer()
{
	return impl->frames[impl->frame_idx].cmd_buffer;
}

vk::Framebuffer PlatformWindow::getFrameBuffer()
{
	return impl->frames[impl->frame_idx].frame_buffer;
}


uvec2 PlatformWindow::getFrameBufferSize() const
{
	return impl->frame_buffer_size;
}

vk::ClearColorValue PlatformWindow::getClearColorValue() const
{
	return impl->clear_color;
}

VKDL_END