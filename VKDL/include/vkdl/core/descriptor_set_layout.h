#pragma once

#include "include_vulkan.h"

VKDL_BEGIN

class DescriptorSetLayout : public std::enable_shared_from_this<DescriptorSetLayout>
{
	friend class DescriptorSetLayoutBuilder;

	VKDL_NOCOPY(DescriptorSetLayout);

	DescriptorSetLayout(vk::DescriptorSetLayout layout);

public:
	~DescriptorSetLayout();

	vk::DescriptorSetLayout getDescriptorSetLayout() const;
	operator vk::DescriptorSetLayout() const;

private:
	vk::DescriptorSetLayout layout;
};

VKDL_END