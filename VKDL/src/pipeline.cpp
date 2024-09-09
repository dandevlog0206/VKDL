#include "../include/vkdl/core/pipeline.h"

#include "../include/vkdl/core/context.h"

VKDL_BEGIN

Pipeline::Pipeline(vk::Pipeline pipeline, std::shared_ptr<PipelineLayout>& layout, std::vector<std::shared_ptr<ShaderModule>>& modules) :
	pipeline(pipeline),
	layout(layout),
	modules(modules)
{
}

Pipeline::~Pipeline()
{
	auto& device = Context::get().device;
	device.destroy(pipeline);
}

vk::Pipeline Pipeline::get()
{
	return pipeline;
}

Pipeline::operator vk::Pipeline() const
{
	return pipeline;
}

PipelineLayout& Pipeline::getPipelineLayout() const
{
	return *layout;
}

VKDL_END
