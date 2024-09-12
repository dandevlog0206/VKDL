#pragma once

#include <queue>
#include "../../include/vkdl/core/context.h"
#include "../../include/vkdl/core/render_states.h"
#include "../../include/vkdl/core/builtin_objects.h"
#include "../../include/vkdl/system/window_event.h"
#include "../../include/vkdl/math/vector_type.h"

VKDL_BEGIN
VKDL_PRIV_BEGIN

struct WindowFrame {
	vk::Image       image;
	vk::ImageView   image_view;
	vk::Framebuffer frame_buffer;

	vk::CommandPool   cmd_pool;
	vk::CommandBuffer cmd_buffer;

	vk::Fence fence;

	RenderStates states;
};

struct FrameSemaphore {
	vk::Semaphore image_acquired;
	vk::Semaphore render_complete;
};

struct PlatformWindowBase
{
	PlatformWindowBase() :
		title("Untitled"),
		position(0, 0),
		size(0, 0),
		frame_buffer_size(0, 0),
		size_limit_min(0, 0),
		size_limit_max(0, 0),
		transparency(0.f),
		visible(false),
		minimized(false),
		maximized(false),
		focussed(false),
		closed(false),
		surface(nullptr),
		swapchain(nullptr),
		present_queue_family_idx(~0),
		present_mode(vk::PresentModeKHR::eFifo),
		image_format(vk::Format::eR8G8B8A8Unorm),
		clear_color(0, 0, 0, 255),
		frame_idx(0),
		semaphore_idx(0),
		platform_window(nullptr),
		parent(nullptr),
		mouse_prev(0, 0),
		update_swapchain(true),
		render_begin(false),
		user_ptr(nullptr)
	{
	}

	std::string title;
	ivec2       position;
	uvec2       size;
	uvec2       frame_buffer_size;
	uvec2       size_limit_min;
	uvec2       size_limit_max;
	float       transparency;
	bool        visible;
	bool        minimized;
	bool        maximized;
	bool        focussed;
	bool        closed;

	vk::SurfaceKHR                    surface;
	vk::SwapchainKHR                  swapchain;
	uint32_t                          present_queue_family_idx;
	vk::SurfaceCapabilitiesKHR        capabilities;
	std::vector<vk::SurfaceFormatKHR> formats;
	std::vector<vk::PresentModeKHR>   present_modes;
	vk::PresentModeKHR                present_mode;
	vk::Format                        image_format;
	vk::ClearColorValue               clear_color;

	std::vector<WindowFrame>    frames;
	std::vector<FrameSemaphore> semaphores;
	std::queue<WindowEvent>     events;

	uint32_t frame_idx;
	uint32_t semaphore_idx;

	PlatformWindow* platform_window;
	PlatformWindow* parent;

	ivec2 mouse_prev;
	bool  update_swapchain;
	bool  render_begin;
	void* user_ptr;

	void init()
	{
		auto& ctx = Context::get();

		auto properties = ctx.physical_device.getQueueFamilyProperties();

		for (uint32_t i = 0; i < properties.size(); ++i) {
			if (ctx.physical_device.getSurfaceSupportKHR(i, surface)) {
				present_queue_family_idx = i;
			}
		}

		capabilities = ctx.physical_device.getSurfaceCapabilitiesKHR(surface);
		formats = ctx.physical_device.getSurfaceFormatsKHR(surface);
		present_modes = ctx.physical_device.getSurfacePresentModesKHR(surface);

		registerBuiltinRenderpass(VKDL_BUILTIN_RENDERPASS0_UUID);
		recreate_swapchain();
	}

	void destroy()
	{
		auto& ctx = Context::get();
		auto& device = ctx.device;

		device.waitIdle();
		destroy_window_frame();
		destroy_frame_semaphores();

		device.destroy(swapchain);
		ctx.instance.destroy(surface);
	}

	void sendEventMsg(WindowEvent& msg)
	{
		platform_window->OnEventRecieved.invoke(*platform_window, msg);
		events.push(msg);
	}

	bool checkPresentModeCompability(vk::PresentModeKHR mode) const
	{
		return std::find(present_modes.begin(), present_modes.end(), mode) != present_modes.end();
	}

	void recreate_swapchain()
	{
		auto& ctx = Context::get();
		auto& device = ctx.device;

		device.waitIdle();
		destroy_window_frame();
		destroy_frame_semaphores();

		capabilities = ctx.physical_device.getSurfaceCapabilitiesKHR(surface);

		auto extent = capabilities.currentExtent;
		auto old_swapchain = swapchain;

		if (extent.width == 0 || extent.height == 0) return;

		vk::SwapchainCreateInfoKHR swapchain_info(
			{},
			surface,
			get_swapchain_image_count(),
			image_format,
			vk::ColorSpaceKHR::eSrgbNonlinear,
			get_swapchain_extent(),
			1,
			vk::ImageUsageFlagBits::eColorAttachment,
			{}, {}, {},
			vk::SurfaceTransformFlagBitsKHR::eIdentity,
			vk::CompositeAlphaFlagBitsKHR::eOpaque,
			present_mode,
			true,
			old_swapchain
		);

		uint32_t queue_families[2] = {
			ctx.graphics_queue_family_idx,
			present_queue_family_idx
		};

		if (ctx.graphics_queue_family_idx == present_queue_family_idx) {
			swapchain_info.imageSharingMode = vk::SharingMode::eExclusive;
			swapchain_info.queueFamilyIndexCount = 0;
			swapchain_info.pQueueFamilyIndices = nullptr;
		}
		else {
			swapchain_info.imageSharingMode = vk::SharingMode::eConcurrent;
			swapchain_info.queueFamilyIndexCount = 2;
			swapchain_info.pQueueFamilyIndices = queue_families;
		}

		swapchain = ctx.device.createSwapchainKHR(swapchain_info);
		device.destroy(old_swapchain);

		create_window_frame();
		create_frame_semaphores();

		update_swapchain = false;
	}

	void acquireSwapchainImage()
	{
		auto& device = Context::get().device;

		if (update_swapchain)
			recreate_swapchain();

		while (true) {
			auto res = device.acquireNextImageKHR(
				swapchain,
				UINT64_MAX,
				semaphores[semaphore_idx].image_acquired,
				nullptr,
				&frame_idx);

			if (res != vk::Result::eSuboptimalKHR && res != vk::Result::eErrorOutOfDateKHR) return;

			recreate_swapchain();
		};
	}

	vk::Extent2D get_swapchain_extent()
	{
		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
			return capabilities.currentExtent;
		}
		else {
			vk::Extent2D actualExtent{ size.x, size.y };

			actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
			actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

			return actualExtent;
		}
	}

	uint32_t get_swapchain_image_count()
	{
		uint32_t image_count = capabilities.minImageCount + 1;

		if (0 < capabilities.maxImageCount && capabilities.maxImageCount < image_count)
			image_count = capabilities.maxImageCount;

		return image_count;
	}

	void create_window_frame()
	{
		auto& ctx = Context::get();
		auto& device = ctx.device;

		auto images = device.getSwapchainImagesKHR(swapchain);

		for (auto& image : images) {
			vk::ImageViewCreateInfo image_view_info = {
				{},
				image,
				vk::ImageViewType::e2D,
				image_format,
				{
					vk::ComponentSwizzle::eIdentity,
					vk::ComponentSwizzle::eIdentity,
					vk::ComponentSwizzle::eIdentity,
					vk::ComponentSwizzle::eIdentity
				},
				{
					vk::ImageAspectFlagBits::eColor,
					0, 1, 0, 1
				}
			};

			auto image_view = device.createImageView(image_view_info);

			vk::FramebufferCreateInfo frame_buffer_info = {
				{},
				ctx.render_passes[VKDL_BUILTIN_RENDERPASS0_UUID]->get(),
				1, &image_view,
				capabilities.currentExtent.width,
				capabilities.currentExtent.height,
				1,
			};

			vk::CommandPoolCreateInfo command_pool_info = {
				vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
				ctx.graphics_queue_family_idx
			};

			auto command_pool = device.createCommandPool(command_pool_info);

			vk::CommandBufferAllocateInfo command_buffer_info = {
				command_pool, vk::CommandBufferLevel::ePrimary, 1
			};

			vk::FenceCreateInfo fence_info = {
				vk::FenceCreateFlagBits::eSignaled
			};

			WindowFrame frame;
			frame.image = image;
			frame.image_view = image_view;
			frame.frame_buffer = device.createFramebuffer(frame_buffer_info);
			frame.cmd_pool = command_pool;
			frame.cmd_buffer = device.allocateCommandBuffers(command_buffer_info).front();
			frame.fence = device.createFence(fence_info);

			frames.push_back(frame);
		}
	}

	void create_frame_semaphores()
	{
		auto& device = Context::get().device;

		for (int i = 0; i < frames.size(); ++i) {
			FrameSemaphore semaphore;

			semaphore.image_acquired = device.createSemaphore({});
			semaphore.render_complete = device.createSemaphore({});

			semaphores.emplace_back(semaphore);
		}

		semaphore_idx = 0;
	}

	void destroy_window_frame()
	{
		auto& device = Context::get().device;

		for (auto& frame : frames) {
			device.destroy(frame.image_view);
			device.destroy(frame.frame_buffer);
			device.freeCommandBuffers(frame.cmd_pool, 1, &frame.cmd_buffer);
			device.destroy(frame.cmd_pool);
			device.destroy(frame.fence);
		}

		frames.clear();
	}

	void destroy_frame_semaphores()
	{
		auto& device = Context::get().device;

		for (auto& semaphore : semaphores) {
			device.destroy(semaphore.image_acquired);
			device.destroy(semaphore.render_complete);
		}

		semaphores.clear();
	}
};

VKDL_PRIV_END
VKDL_END