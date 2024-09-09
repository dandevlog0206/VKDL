#pragma once

#include "../core/descriptor_set_layout.h"

VKDL_BEGIN

class PipelineLayout : public std::enable_shared_from_this<PipelineLayout>
{
	friend class PipelineLayoutBuilder;

	VKDL_NOCOPY(PipelineLayout);

	PipelineLayout(vk::PipelineLayout layout, std::vector<std::shared_ptr<DescriptorSetLayout>>& desc_layouts);

public:
	~PipelineLayout();

	DescriptorSetLayout& getDescriptorSetLayout(uint32_t set_idx) const;

	vk::PipelineLayout getPipelineLayout() const;
	operator vk::PipelineLayout() const;

private:
	vk::PipelineLayout                                layout;
	std::vector<std::shared_ptr<DescriptorSetLayout>> desc_layouts;
};

VKDL_END