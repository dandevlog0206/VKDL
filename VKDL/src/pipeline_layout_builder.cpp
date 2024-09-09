#include "../include/vkdl/builder/pipeline_layout_builder.h"

#include "../include/vkdl/core/context.h"

VKDL_BEGIN

PipelineLayoutBuilder::PipelineLayoutBuilder()
{
	clear();
}

void PipelineLayoutBuilder::clear()
{
	desc_layouts.clear();
	pc_ranges.clear();
}

PipelineLayoutBuilder& PipelineLayoutBuilder::addDescriptorSetLayout(std::shared_ptr<DescriptorSetLayout>& layout)
{
	desc_layout_ptrs.emplace_back(layout);
	desc_layouts.emplace_back(*layout);
	return *this;
}

PipelineLayoutBuilder& PipelineLayoutBuilder::addPushConstant(vk::ShaderStageFlagBits stage, uint32_t offset, uint32_t size)
{
	pc_ranges.emplace_back(stage, offset, size);
	return *this;
}

std::shared_ptr<PipelineLayout> PipelineLayoutBuilder::build()
{
	auto& ctx    = Context::get();
	auto& device = ctx.device;

	vk::PipelineLayoutCreateInfo info = {};
	info.setLayoutCount         = (uint32_t)desc_layouts.size();
	info.pSetLayouts            = desc_layouts.data();
	info.pushConstantRangeCount = (uint32_t)pc_ranges.size();
	info.pPushConstantRanges    = pc_ranges.data();

	return std::shared_ptr<PipelineLayout>(new PipelineLayout(device.createPipelineLayout(info), desc_layout_ptrs));
}

VKDL_END