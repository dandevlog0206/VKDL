#pragma once

#include <map>
#include "include_vulkan.h"

VKDL_BEGIN

class ShaderModule : public std::enable_shared_from_this<ShaderModule>
{
	ShaderModule(vk::ShaderModule module);

	VKDL_NOCOPY(ShaderModule);

public:
	static std::shared_ptr<ShaderModule> loadFromMemory(const uint32_t* code, size_t size_in_byte);
	
	~ShaderModule();

	vk::PipelineShaderStageCreateInfo makeShaderStageCreateInfo(vk::ShaderStageFlagBits stage, const char* name = "main");

	vk::PipelineShaderStageCreateInfo operator[](vk::ShaderStageFlagBits stage) const;

private:
	std::map<vk::ShaderStageFlagBits, vk::PipelineShaderStageCreateInfo> stages;
	std::vector<std::string>                                             names;
	vk::ShaderModule                                                     module;
};

VKDL_END