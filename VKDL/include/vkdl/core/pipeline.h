#pragma once

#include "pipeline_layout.h"
#include "shader_module.h"

VKDL_BEGIN

class Pipeline : public std::enable_shared_from_this<Pipeline>
{
	friend class PipelineBuilder;

	VKDL_NOCOPY(Pipeline);

	Pipeline(vk::Pipeline pipeline, std::shared_ptr<PipelineLayout>& layout, std::vector<std::shared_ptr<ShaderModule>>& modules);

public:
	~Pipeline();
	
	PipelineLayout& getPipelineLayout() const;

	vk::Pipeline get();
	operator vk::Pipeline() const;

private:
	vk::Pipeline                               pipeline;
	std::shared_ptr<PipelineLayout>            layout;
	std::vector<std::shared_ptr<ShaderModule>> modules;
};

VKDL_END