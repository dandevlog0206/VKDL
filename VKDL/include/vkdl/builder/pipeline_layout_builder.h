#pragma once

#include "../core/pipeline_layout.h"
#include "../core/descriptor_set_layout.h"

VKDL_BEGIN

class PipelineLayoutBuilder
{
public:
	PipelineLayoutBuilder();

	void clear();

	PipelineLayoutBuilder& addDescriptorSetLayout(std::shared_ptr<DescriptorSetLayout>& layout);
	PipelineLayoutBuilder& addPushConstant(vk::ShaderStageFlagBits stage, uint32_t offset, uint32_t size);

	std::shared_ptr<PipelineLayout> build();

private:
	std::vector<vk::DescriptorSetLayout>              desc_layouts;
	std::vector<std::shared_ptr<DescriptorSetLayout>> desc_layout_ptrs;
	std::vector<vk::PushConstantRange>                pc_ranges;
};

VKDL_END