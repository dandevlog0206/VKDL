#pragma once

#include "include_vulkan.h"

VKDL_BEGIN

class RenderPass : public std::enable_shared_from_this<RenderPass>
{
	friend class RenderPassBuilder;

	VKDL_NOCOPY(RenderPass);

	RenderPass(vk::RenderPass renderpass);
public:
	~RenderPass();

	vk::RenderPass get() const;
	operator vk::RenderPass() const;

private:
	vk::RenderPass renderpass;
};

VKDL_END