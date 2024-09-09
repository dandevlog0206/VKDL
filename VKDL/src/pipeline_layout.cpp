#include "../include/vkdl/core/pipeline_layout.h"

#include "../include/vkdl/core/context.h"

VKDL_BEGIN

PipelineLayout::PipelineLayout(vk::PipelineLayout layout, std::vector<std::shared_ptr<DescriptorSetLayout>>& desc_layouts) :
	layout(layout),
	desc_layouts(desc_layouts)
{
}

PipelineLayout::~PipelineLayout()
{
	auto& device = Context::get().device;
	device.destroy(layout);
}

DescriptorSetLayout& PipelineLayout::getDescriptorSetLayout(uint32_t set_idx) const
{
	return *desc_layouts[set_idx];
}

vk::PipelineLayout PipelineLayout::getPipelineLayout() const
{
	return layout;
}

PipelineLayout::operator vk::PipelineLayout() const
{
	return layout;
}

VKDL_END