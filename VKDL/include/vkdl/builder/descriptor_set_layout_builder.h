#include "../core/descriptor_set_layout.h"

VKDL_BEGIN

class DescriptorSetLayoutBuilder
{
public:
	DescriptorSetLayoutBuilder();

	void clear();

	DescriptorSetLayoutBuilder& setBinding(uint32_t binding);
	DescriptorSetLayoutBuilder& setDescriptorType(vk::DescriptorType type);
	DescriptorSetLayoutBuilder& setShaderStage(vk::ShaderStageFlagBits stage);
	DescriptorSetLayoutBuilder& addSampler(vk::Sampler sampler);
	DescriptorSetLayoutBuilder& pushCurrentBinding();

	std::shared_ptr<DescriptorSetLayout> build();

private:
	std::vector<vk::DescriptorSetLayoutBinding> bindings;
	std::vector<vk::Sampler>                    samplers;
	vk::DescriptorSetLayoutBinding              curr_binding;
};

VKDL_END