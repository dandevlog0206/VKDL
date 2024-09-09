#include "../include/vkdl/core/context.h"

#include "../include/vkdl/core/exception.h"

#ifdef VKDL_PLATFORM_WINDOWS
#define PLATFORM_SURFACE_EXT_NAME "VK_KHR_win32_surface"
#endif

static bool check_layer_support(const char* layer_name) {
	auto props = vk::enumerateInstanceLayerProperties();

	for (const auto& prop : props) {
		if (!strcmp(prop.layerName.data(), layer_name)) return true;
	}
	return false;
}

static const char* get_message_type_str(VkDebugReportFlagsEXT flags)
{
	switch (flags) {
	case VK_DEBUG_REPORT_INFORMATION_BIT_EXT:         return "[Information]";
	case VK_DEBUG_REPORT_WARNING_BIT_EXT:             return "[Warning]";
	case VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT: return "[PerformanceWarning]";
	case VK_DEBUG_REPORT_ERROR_BIT_EXT:               return "[Error]";
	case VK_DEBUG_REPORT_DEBUG_BIT_EXT:               return "[Debug]";
	default:                                          return "[Unknown]";
	}
}

static VKAPI_ATTR VkBool32 VKAPI_CALL debug_report_callback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType, uint64_t object, size_t location, int32_t messageCode, const char* pLayerPrefix, const char* pMessage, void* pUserData)
{
	(void)flags; (void)object; (void)location; (void)messageCode; (void)pUserData; (void)pLayerPrefix; // Unused arguments

	fprintf(stderr, "%sVulkan Debug report from ObjectType: %i\nMessage: %s\n\n", get_message_type_str(flags), objectType, pMessage);
	
	if (flags != VkDebugReportFlagBitsEXT::VK_DEBUG_REPORT_INFORMATION_BIT_EXT)
		int a = 0;

	return VK_FALSE;
}

static struct DebugCallbackDispatcher {
	void create(vk::Instance instance) {
		vkCreateDebugReportCallbackEXT = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT");
		vkDestroyDebugReportCallbackEXT = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT");
	}

	static int getVkHeaderVersion() { return VK_HEADER_VERSION; }

	PFN_vkCreateDebugReportCallbackEXT  vkCreateDebugReportCallbackEXT  = nullptr;
	PFN_vkDestroyDebugReportCallbackEXT vkDestroyDebugReportCallbackEXT = nullptr;
} debug_callback_dispatcher;

VKDL_BEGIN

ContextCreator::ContextCreator() :
	physical_device_type(vk::PhysicalDeviceType::eDiscreteGpu),
	debug_level(0)
{
}

ContextCreator& ContextCreator::setApplicationInfo(const vk::ApplicationInfo& app_info)
{
	this->app_info = app_info;
	return *this;
}

ContextCreator& ContextCreator::addLayer(const char* layer_name)
{
	layers.push_back(layer_name);
	return *this;
}

ContextCreator& ContextCreator::addExtension(const char* ext_name)
{
	extensions.push_back(ext_name);
	return *this;
}

ContextCreator& ContextCreator::addDeviceExtension(const char* ext_name)
{
	device_extensions.push_back(ext_name);
	return *this;
}

ContextCreator& ContextCreator::setPhysicalDeviceType(vk::PhysicalDeviceType type)
{
	physical_device_type = type;
	return *this;
}

ContextCreator& ContextCreator::enableDebug(uint32_t level)
{
	VKDL_CHECK_MSG(level < 6, "Maximum debug level is 5");
	debug_level = level;
	
	if (!check_layer_support("VK_LAYER_KHRONOS_validation"))
		VKDL_ERROR("VK_LAYER_KHRONOS_validation not supported");

	layers.push_back("VK_LAYER_KHRONOS_validation");
	extensions.push_back("VK_EXT_debug_report");

	return *this;
}

std::unique_ptr<Context> ContextCreator::create()
{
	extensions.push_back(PLATFORM_SURFACE_EXT_NAME);
	extensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
	device_extensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

	return std::unique_ptr<Context>(new Context(*this));
}

Context* Context::context_inst = nullptr;

Context::Context(const ContextCreator& creator) :
	graphics_queue_family_idx((uint32_t)-1),
	debug_callback(nullptr)
{
	if (context_inst) VKDL_ERROR("VKDL Context already exists");
	context_inst = this;

	vk::InstanceCreateInfo inst_info({}, &creator.app_info, creator.layers, creator. extensions);
	instance = vk::createInstance(inst_info);

	setDebugCallback(creator.debug_level);
	selectPhysicalDevice(creator.physical_device_type);
	createDevice(creator.device_extensions);
	createDescriptorPool();
	createCommandPool();

	pipeline_cache = device.createPipelineCache({ {}, 0, nullptr });
}

Context::~Context()
{
	device.waitIdle();

	for (auto& pipeline : pipelines)
		pipeline.second.reset();

	for (auto& render_pass : render_passes)
		render_pass.second.reset();

	device.destroy(command_pool);
	device.destroy(descriptor_pool);
	device.destroy(pipeline_cache);
	device.destroy();

	if (debug_callback)
		instance.destroyDebugReportCallbackEXT(debug_callback, nullptr, debug_callback_dispatcher);

	instance.destroy();

	context_inst = nullptr;
}

vk::CommandBuffer Context::beginSingleTimeCommmand()
{
	vk::CommandBufferAllocateInfo buffer_info = {
		command_pool,
		vk::CommandBufferLevel::ePrimary,
		1,
	};

	auto cmd_buffer = device.allocateCommandBuffers(buffer_info).front();

	cmd_buffer.begin({ vk::CommandBufferUsageFlagBits::eOneTimeSubmit });

	return cmd_buffer;
}

void Context::endSingleTimeCommmand(vk::CommandBuffer cmd_buffer)
{
	cmd_buffer.end();

	vk::SubmitInfo submit_info = {
		0, nullptr,
		nullptr,
		1, &cmd_buffer
	};

	VK_CHECK(queues[graphics_queue_family_idx].submit(1, &submit_info, nullptr));
	queues[graphics_queue_family_idx].waitIdle();
	device.freeCommandBuffers(command_pool, 1, &cmd_buffer);
}

void Context::transitionImageLayout(vk::CommandBuffer cmd_buffer, vk::Image image, vk::Format format, vk::ImageLayout old_layout, vk::ImageLayout new_layout)
{
	vk::ImageMemoryBarrier barrier = {
		vk::AccessFlagBits::eNone,
		vk::AccessFlagBits::eNone,
		old_layout,
		new_layout,
		VK_QUEUE_FAMILY_IGNORED,
		VK_QUEUE_FAMILY_IGNORED,
		image,
		{
			vk::ImageAspectFlagBits::eColor,
			0, 1, 0, 1
		}
	};


	vk::PipelineStageFlags src_stage;
	vk::PipelineStageFlags dst_stage;

	if (old_layout == vk::ImageLayout::eUndefined && new_layout == vk::ImageLayout::eTransferDstOptimal) {
		barrier.srcAccessMask = vk::AccessFlagBits::eNone;
		barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

		src_stage = vk::PipelineStageFlagBits::eTopOfPipe;
		dst_stage = vk::PipelineStageFlagBits::eTransfer;
	} else if (old_layout == vk::ImageLayout::eUndefined && new_layout == vk::ImageLayout::eTransferSrcOptimal) {
		barrier.srcAccessMask = vk::AccessFlagBits::eNone;
		barrier.dstAccessMask = vk::AccessFlagBits::eTransferRead;

		src_stage = vk::PipelineStageFlagBits::eTopOfPipe;
		dst_stage = vk::PipelineStageFlagBits::eTransfer;
	} else if (old_layout == vk::ImageLayout::eTransferDstOptimal && new_layout == vk::ImageLayout::eShaderReadOnlyOptimal) {
		barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
		barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

		src_stage = vk::PipelineStageFlagBits::eTransfer;
		dst_stage = vk::PipelineStageFlagBits::eFragmentShader;
	} else if (old_layout == vk::ImageLayout::eUndefined && new_layout == vk::ImageLayout::eShaderReadOnlyOptimal) {
		barrier.srcAccessMask = vk::AccessFlagBits::eNone;
		barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

		src_stage = vk::PipelineStageFlagBits::eTopOfPipe;
		dst_stage = vk::PipelineStageFlagBits::eFragmentShader;
	} else if (old_layout == vk::ImageLayout::eTransferSrcOptimal && new_layout == vk::ImageLayout::eShaderReadOnlyOptimal) {
		barrier.srcAccessMask = vk::AccessFlagBits::eTransferRead;
		barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

		src_stage = vk::PipelineStageFlagBits::eTransfer;
		dst_stage = vk::PipelineStageFlagBits::eFragmentShader;
	} else {
		VKDL_ERROR("unsupported layout transition");
	}

	cmd_buffer.pipelineBarrier(
		src_stage,
		dst_stage,
		{},
		0, nullptr,
		0, nullptr,
		1, &barrier);
}

vk::DeviceSize Context::alignMemorySize(vk::DeviceSize size) const
{
	auto align = physical_device_props.limits.nonCoherentAtomSize;
	return ((size - 1) / align + 1) * align;
}

uint32_t Context::findMemoryType(uint32_t type_filter, vk::MemoryPropertyFlags props) const
{
	const auto& mem_props = physical_device_memory_props;
	for (uint32_t i = 0; i < mem_props.memoryTypeCount; i++) {
		if (type_filter & (1 << i) && (mem_props.memoryTypes[i].propertyFlags & props) == props) {
			return i;
		}
	}

	VKDL_ERROR("failed to find suitable memory type");
	return 0;
}

bool Context::hasPipeline(const UUID& pipeline_uuid) const
{
	return pipelines.find(pipeline_uuid) != pipelines.end();
}

void Context::registerPipeline(const UUID& pipeline_uuid, std::shared_ptr<Pipeline>& pipeline)
{
	pipelines.insert(std::make_pair(pipeline_uuid, pipeline));
}

Pipeline& Context::getPipeline(const UUID& uuid)
{
	return *pipelines[uuid];
}

bool Context::hasRenderPass(const UUID& renderpass_uuid) const
{
	return render_passes.find(renderpass_uuid) != render_passes.end();;
}

void Context::registerRenderPass(const UUID& renderpass_uuid, std::shared_ptr<RenderPass>& renderpass)
{
	render_passes.insert(std::make_pair(renderpass_uuid, renderpass));
}

RenderPass& Context::getRenderpass(const UUID& uuid)
{
	return *render_passes[uuid];
}

void Context::setDebugCallback(uint32_t debug_level)
{
	if (0 < debug_level) {
		vk::DebugReportCallbackCreateInfoEXT image_info = {};
		image_info.flags = 
			(debug_level >= 1 ? vk::DebugReportFlagBitsEXT::eError : vk::DebugReportFlagBitsEXT(0)) |
			(debug_level >= 2 ? vk::DebugReportFlagBitsEXT::eWarning : vk::DebugReportFlagBitsEXT(0)) |
			(debug_level >= 3 ? vk::DebugReportFlagBitsEXT::ePerformanceWarning : vk::DebugReportFlagBitsEXT(0)) |
			(debug_level >= 4 ? vk::DebugReportFlagBitsEXT::eDebug : vk::DebugReportFlagBitsEXT(0)) |
			(debug_level >= 5 ? vk::DebugReportFlagBitsEXT::eInformation : vk::DebugReportFlagBitsEXT(0));
		image_info.pfnCallback = debug_report_callback;
		image_info.pUserData   = nullptr;

		debug_callback_dispatcher.create(instance);
		debug_callback = instance.createDebugReportCallbackEXT(image_info, nullptr, debug_callback_dispatcher);
	}
}

void Context::selectPhysicalDevice(vk::PhysicalDeviceType preffered_type)
{
	auto physical_devices = instance.enumeratePhysicalDevices();

	for (const auto& device : physical_devices) {
		auto type = device.getProperties().deviceType;

		if (type == preffered_type) {
			physical_device = device;
			break;
		}
	}

	if (!physical_device)
		physical_device = physical_devices.front();

	physical_device_props = physical_device.getProperties();
	physical_device_memory_props = physical_device.getMemoryProperties();
}

void Context::createDevice(const std::vector<const char*>& device_extensions)
{
	auto properties      = physical_device.getQueueFamilyProperties();
	float queue_priority = 1.f;

	std::vector<vk::DeviceQueueCreateInfo> queue_infos;

	for (uint32_t i = 0; i < properties.size(); ++i) {
		if (properties[i].queueFlags & vk::QueueFlagBits::eGraphics)
			graphics_queue_family_idx = i;

		vk::DeviceQueueCreateInfo queue_info = {
			{},
			i,
			1,
			&queue_priority
		};

		queue_infos.push_back(queue_info);
	}

	device = physical_device.createDevice({ {}, queue_infos, {}, device_extensions });

	for (size_t i = 0; i < properties.size(); ++i)
		queues.push_back(device.getQueue(graphics_queue_family_idx, 0));
}

void Context::createDescriptorPool()
{
	std::array<vk::DescriptorPoolSize, 1> pool_sizes = {{
		{ vk::DescriptorType::eCombinedImageSampler, 100 }
	}};

	vk::DescriptorPoolCreateInfo pool_info = {
		vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
		100,
		pool_sizes
	};

	descriptor_pool = device.createDescriptorPool(pool_info);
}

void Context::createCommandPool()
{
	vk::CommandPoolCreateInfo pool_info = {
		vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
		graphics_queue_family_idx,
	};

	command_pool = device.createCommandPool(pool_info);
}

VKDL_END