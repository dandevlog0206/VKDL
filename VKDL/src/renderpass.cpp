#include "../include/vkdl/core/renderpass.h"

#include "../include/vkdl/core/context.h"

VKDL_BEGIN

RenderPass::RenderPass(vk::RenderPass renderpass) :
	renderpass(renderpass)
{
}

RenderPass::~RenderPass()
{
	auto& device = Context::get().device;
	device.destroy(renderpass);
}

vk::RenderPass RenderPass::get() const
{
	return renderpass;
}

RenderPass::operator vk::RenderPass() const
{
	return renderpass;
}

VKDL_END