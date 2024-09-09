#include "../include/vkdl/core/shader_module.h"

#include "../include/vkdl/core/context.h"

VKDL_BEGIN

ShaderModule::ShaderModule(vk::ShaderModule module) :
	module(module)
{
}

std::shared_ptr<ShaderModule> ShaderModule::loadFromMemory(const uint32_t* code, size_t size_in_byte)
{
	auto& device = Context::get().device;

	vk::ShaderModuleCreateInfo image_info = {};
	image_info.codeSize = size_in_byte;
	image_info.pCode    = code;

	return std::shared_ptr<ShaderModule>(new ShaderModule(device.createShaderModule(image_info)));
}

ShaderModule::~ShaderModule()
{
	auto& device = Context::get().device;
	device.destroy(module);
}

vk::PipelineShaderStageCreateInfo ShaderModule::makeShaderStageCreateInfo(vk::ShaderStageFlagBits stage, const char* name)
{
	VKDL_CHECK_MSG(stages.find(stage) == stages.end(), "Cannot create shader stage with duplicate stage");

	auto& name_str = names.emplace_back(name);

	vk::PipelineShaderStageCreateInfo image_info = {};
	image_info.stage  = stage;
	image_info.module = module;
	image_info.pName  = name_str.c_str();

	return stages.insert(std::make_pair(stage, image_info)).first->second;
}

vk::PipelineShaderStageCreateInfo ShaderModule::operator[](vk::ShaderStageFlagBits stage) const
{
	return stages.find(stage)->second;
}

VKDL_END
