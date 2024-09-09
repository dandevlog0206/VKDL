#include "../include/vkdl/core/descriptor_set_layout.h"

#include "../include/vkdl/core/context.h"

VKDL_BEGIN

DescriptorSetLayout::DescriptorSetLayout(vk::DescriptorSetLayout layout) :
	layout(layout)
{
}

DescriptorSetLayout::~DescriptorSetLayout()
{
	auto& device = Context::get().device;
	device.destroy(layout);
}

vk::DescriptorSetLayout DescriptorSetLayout::getDescriptorSetLayout() const
{
	return layout;
}

DescriptorSetLayout::operator vk::DescriptorSetLayout() const
{
	return layout;
}

VKDL_END