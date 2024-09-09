#include "../include/vkdl/builder/descriptor_set_layout_builder.h"

#include "../include/vkdl/core/context.h"

VKDL_BEGIN

DescriptorSetLayoutBuilder::DescriptorSetLayoutBuilder()
{
	clear();
}

void DescriptorSetLayoutBuilder::clear()
{
	bindings.clear();
	samplers.clear();
	curr_binding = vk::DescriptorSetLayoutBinding();
}

DescriptorSetLayoutBuilder& DescriptorSetLayoutBuilder::setBinding(uint32_t binding)
{
	curr_binding.binding = binding;
	return *this;
}

DescriptorSetLayoutBuilder& DescriptorSetLayoutBuilder::setDescriptorType(vk::DescriptorType type)
{
	curr_binding.descriptorType = type;
	return *this;
}

DescriptorSetLayoutBuilder& DescriptorSetLayoutBuilder::setShaderStage(vk::ShaderStageFlagBits stage)
{
	curr_binding.stageFlags = stage;
	return *this;
}

DescriptorSetLayoutBuilder& DescriptorSetLayoutBuilder::addSampler(vk::Sampler sampler)
{
	if (sampler != nullptr) {
		samplers.push_back(sampler);
		curr_binding.pImmutableSamplers = (vk::Sampler*)-1;
	}
	curr_binding.descriptorCount++;
	return *this;
}

DescriptorSetLayoutBuilder& DescriptorSetLayoutBuilder::pushCurrentBinding()
{
	auto binding = bindings.emplace_back(curr_binding);
	curr_binding = vk::DescriptorSetLayoutBinding();
	return *this;
}

std::shared_ptr<DescriptorSetLayout> DescriptorSetLayoutBuilder::build()
{
	auto& device = Context::get().device;

	uint32_t sampler_idx = 0;

	for (auto& binding : bindings) {
		if (binding.descriptorType == vk::DescriptorType::eSampler ||
			binding.descriptorType == vk::DescriptorType::eCombinedImageSampler) {
			if (binding.pImmutableSamplers != 0) {
				binding.pImmutableSamplers = &samplers[sampler_idx];
				sampler_idx += binding.descriptorCount;
			}
		}
	}

	vk::DescriptorSetLayoutCreateInfo info = {};
	info.bindingCount = (uint32_t)bindings.size();
	info.pBindings    = bindings.data();

	return std::shared_ptr<DescriptorSetLayout>(new DescriptorSetLayout(device.createDescriptorSetLayout(info)));
}

VKDL_END