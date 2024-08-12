#pragma once

#include "include_vulkan.h"

VKDL_BEGIN

class Context {
	VKDL_NOCOPY(Context)
	VKDL_NOMOVE(Context)
	VKDL_NOCOPYASS(Context)
	VKDL_NOMOVEASS(Context)

public:
	static VKDL_INLINE Context& get() { return *context_inst; }

	Context(const vk::ApplicationInfo& info, std::vector<const char*> layers, std::vector<const char*> extensions, bool enable_debug = false);
	~Context();

	vk::CommandBuffer beginSingleTimeCommmand();
	void endSingleTimeCommmand(vk::CommandBuffer cmd_buffer);

	void transitionImageLayout(vk::CommandBuffer cmd_buffer, vk::Image image, vk::Format format, vk::ImageLayout old_layout, vk::ImageLayout new_layout);
	void copyBuffer(vk::CommandBuffer cmd_buffer, vk::Buffer dst, vk::Buffer src, vk::DeviceSize dst_off, vk::DeviceSize src_off, vk::DeviceSize size);;

	std::pair<vk::DeviceMemory, vk::Buffer> createBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags props);

	vk::DeviceMemory allocateDeviceMemory(vk::Buffer buffer, vk::DeviceSize size);
	vk::DeviceSize alignMemorySize(vk::DeviceSize size) const;
	uint32_t findMemoryType(uint32_t type_filter, vk::MemoryPropertyFlags props) const;

	vk::Instance instance;

	vk::PhysicalDevice                 physical_device;
	vk::PhysicalDeviceProperties       physical_device_props;
	vk::PhysicalDeviceMemoryProperties physical_device_memory_props;
	uint32_t                           graphics_queue_family_idx;

	vk::Device             device;
	vk::RenderPass         renderpass;
	vk::PipelineCache      pipeline_cache;
	vk::DescriptorPool     descriptor_pool;
	std::vector<vk::Queue> queues;
	vk::CommandPool        command_pool;

	vk::DebugReportCallbackEXT debug_callback;

private:
	static Context* context_inst;
};

VKDL_END