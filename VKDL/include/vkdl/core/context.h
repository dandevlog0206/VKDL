#pragma once

#include <map>
#include "renderpass.h"
#include "pipeline.h"
#include "../util/uuid.h"

VKDL_BEGIN

class Context;

class ContextCreator
{
	friend class Context;

public:
	ContextCreator();

	ContextCreator& setApplicationInfo(const vk::ApplicationInfo& app_info);
	ContextCreator& addLayer(const char* layer_name);
	ContextCreator& addExtension(const char* ext_name);
	ContextCreator& addDeviceExtension(const char* ext_name);
	ContextCreator& setPhysicalDeviceType(vk::PhysicalDeviceType type);
	ContextCreator& enableDebug(uint32_t level = 5);

	VKDL_NODISCARD std::unique_ptr<Context> create();

private:
	vk::ApplicationInfo      app_info;
	std::vector<const char*> layers;
	std::vector<const char*> extensions;
	std::vector<const char*> device_extensions;
	vk::PhysicalDeviceType   physical_device_type;
	uint32_t                 debug_level;
};

class Context
{
	friend class ContextCreator;

	VKDL_NOCOPY(Context)
	VKDL_NOMOVE(Context)
	VKDL_NOCOPYASS(Context)
	VKDL_NOMOVEASS(Context)

	Context(const ContextCreator& creator);

public:
	static VKDL_INLINE Context& get() { return *context_inst; }

	~Context();

	vk::CommandBuffer beginSingleTimeCommmand();
	void endSingleTimeCommmand(vk::CommandBuffer cmd_buffer);

	void transitionImageLayout(vk::CommandBuffer cmd_buffer, vk::Image image, vk::Format format, vk::ImageLayout old_layout, vk::ImageLayout new_layout);

	vk::DeviceSize alignMemorySize(vk::DeviceSize size) const;
	uint32_t findMemoryType(uint32_t type_filter, vk::MemoryPropertyFlags props) const;

	bool hasPipeline(const UUID& pipeline_uuid) const;
	void registerPipeline(const UUID& pipeline_uuid, std::shared_ptr<Pipeline>& pipeline);
	Pipeline& getPipeline(const UUID& uuid);
	bool hasRenderPass(const UUID& renderpass_uuid) const;
	void registerRenderPass(const UUID& renderpass_uuid, std::shared_ptr<RenderPass>& renderpass);
	RenderPass& getRenderpass(const UUID& uuid);
	
	vk::Instance instance;

	vk::PhysicalDevice                 physical_device;
	vk::PhysicalDeviceProperties       physical_device_props;
	vk::PhysicalDeviceMemoryProperties physical_device_memory_props;

	vk::Device             device;
	std::vector<vk::Queue> queues;
	uint32_t               graphics_queue_family_idx;
	vk::DescriptorPool     descriptor_pool;
	vk::CommandPool        command_pool;
	vk::PipelineCache      pipeline_cache;

	std::map<UUID, std::shared_ptr<Pipeline>>   pipelines;
	std::map<UUID, std::shared_ptr<RenderPass>> render_passes;

	vk::DebugReportCallbackEXT debug_callback;

private:
	void setDebugCallback(uint32_t debug_level);
	void selectPhysicalDevice(vk::PhysicalDeviceType preffered_type);
	void createDevice(const std::vector<const char*>& device_extensions);
	void createDescriptorPool();
	void createCommandPool();

private:
	static Context* context_inst;
};

VKDL_END