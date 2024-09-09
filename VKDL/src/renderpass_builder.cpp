#include "../include/vkdl/builder/renderpass_builder.h"

#include "../include/vkdl/core/context.h"

VKDL_BEGIN

void RenderPassBuilder::clear()
{
	attachments.clear();
	subpasses.clear();
	input_attachments.clear();
	color_attachments.clear();
	resolve_attachments.clear();
	depth_stencil_attachments.clear();
	preserve_attachments.clear();
	dependencies.clear();
	curr_attachment = vk::AttachmentDescription();
	curr_subpass    = vk::SubpassDescription();
	curr_dependency = vk::SubpassDependency();
}

RenderPassBuilder& RenderPassBuilder::setAttachmentFormat(vk::Format format)
{
	curr_attachment.format = format;
	return *this;
}

RenderPassBuilder& RenderPassBuilder::setAttachmentSampleCount(vk::SampleCountFlagBits samples)
{
	curr_attachment.samples = samples;
	return *this;
}

RenderPassBuilder& RenderPassBuilder::setAttachmentLoadOp(vk::AttachmentLoadOp op)
{
	curr_attachment.loadOp = op;
	return *this;
}

RenderPassBuilder& RenderPassBuilder::setAttachmentStoreOp(vk::AttachmentStoreOp op)
{
	curr_attachment.storeOp = op;
	return *this;
}

RenderPassBuilder& RenderPassBuilder::setAttachmentStencilLoadOp(vk::AttachmentLoadOp op)
{
	curr_attachment.stencilLoadOp = op;
	return *this;
}

RenderPassBuilder& RenderPassBuilder::setAttachmentStencilStoreOp(vk::AttachmentStoreOp op)
{
	curr_attachment.stencilStoreOp = op;
	return *this;
}

RenderPassBuilder& RenderPassBuilder::setAttachmentInitialLayout(vk::ImageLayout layout)
{
	curr_attachment.initialLayout = layout;
	return *this;
}

RenderPassBuilder& RenderPassBuilder::setAttachmentFinalLayout(vk::ImageLayout layout)
{
	curr_attachment.finalLayout = layout;
	return *this;
}

RenderPassBuilder& RenderPassBuilder::pushCurrentAttachment()
{
	attachments.push_back(curr_attachment);
	curr_attachment = vk::AttachmentDescription();
	return *this;
}

RenderPassBuilder& RenderPassBuilder::setSubpassPipelineBindPoint(vk::PipelineBindPoint bind_point)
{
	curr_subpass.pipelineBindPoint = bind_point;
	return *this;
}

RenderPassBuilder& RenderPassBuilder::addSubpassInputAttachment(uint32_t attachment, vk::ImageLayout layout)
{
	input_attachments.emplace_back(attachment, layout);
	curr_subpass.inputAttachmentCount++;
	return *this;
}

RenderPassBuilder& RenderPassBuilder::addSubpassColorAttachment(uint32_t attachment, vk::ImageLayout layout)
{
	color_attachments.emplace_back(attachment, layout);
	curr_subpass.colorAttachmentCount++;
	return *this;
}

RenderPassBuilder& RenderPassBuilder::addSubpassResolveAttachment(uint32_t attachment, vk::ImageLayout layout)
{
	resolve_attachments.emplace_back(attachment, layout);
	curr_subpass.pResolveAttachments = (vk::AttachmentReference*)~0;
	return *this;
}

RenderPassBuilder& RenderPassBuilder::setSubpassDepthStencilAttachment(uint32_t attachment, vk::ImageLayout layout)
{
	if (curr_subpass.pDepthStencilAttachment == nullptr) {
		depth_stencil_attachments.emplace_back(attachment, layout);
		curr_subpass.pDepthStencilAttachment = (vk::AttachmentReference*)~0;
	} else {
		depth_stencil_attachments.back() = vk::AttachmentReference(attachment, layout);
	}
	return *this;
}

RenderPassBuilder& RenderPassBuilder::addSubpassPreserveAttachment(uint32_t attachment)
{
	preserve_attachments.emplace_back(attachment);
	curr_subpass.preserveAttachmentCount++;
	return *this;
}

RenderPassBuilder& RenderPassBuilder::pushCurrentSubpass()
{
	subpasses.push_back(curr_subpass);
	curr_subpass = vk::SubpassDescription();
	return *this;
}

RenderPassBuilder& RenderPassBuilder::setDependencySrc(uint32_t subpass, vk::PipelineStageFlags stage, vk::AccessFlags access_mask)
{
	curr_dependency.srcSubpass    = subpass;
	curr_dependency.srcStageMask  = stage;
	curr_dependency.srcAccessMask = access_mask;
	return *this;
}

RenderPassBuilder& RenderPassBuilder::setDependencyDst(uint32_t subpass, vk::PipelineStageFlags stage, vk::AccessFlags access_mask)
{
	curr_dependency.dstSubpass    = subpass;
	curr_dependency.dstStageMask  = stage;
	curr_dependency.dstAccessMask = access_mask;
	return *this;
}

RenderPassBuilder& RenderPassBuilder::setDependencyFlags(vk::DependencyFlagBits flags)
{
	curr_dependency.dependencyFlags = flags;
	return *this;
}

RenderPassBuilder& RenderPassBuilder::pushCurrentDependency()
{
	dependencies.push_back(curr_dependency);
	curr_dependency = vk::SubpassDependency();
	return *this;
}

std::shared_ptr<RenderPass> RenderPassBuilder::build()
{
	auto& device = Context::get().device;

	uint32_t input_idx         = 0;
	uint32_t color_idx         = 0;
	uint32_t resolve_idx       = 0;
	uint32_t depth_stencil_idx = 0;
	uint32_t preserve_idx      = 0;

	for (auto& subpass : subpasses) {
		if (0 < subpass.inputAttachmentCount) {
			subpass.pInputAttachments = &input_attachments[input_idx];
			input_idx += subpass.inputAttachmentCount;
		}
		if (0 < subpass.colorAttachmentCount) {
			subpass.pColorAttachments = &color_attachments[color_idx];
			color_idx += subpass.colorAttachmentCount;
			if (subpass.pResolveAttachments != nullptr) {
				subpass.pResolveAttachments = &resolve_attachments[resolve_idx];
				resolve_idx += subpass.colorAttachmentCount;
			}
		}
		if (subpass.pDepthStencilAttachment != nullptr) {
			subpass.pDepthStencilAttachment = &depth_stencil_attachments[depth_stencil_idx];
			depth_stencil_idx++;
		}
		if (0 < subpass.preserveAttachmentCount) {
			subpass.pPreserveAttachments = &preserve_attachments[preserve_idx];
			preserve_idx += subpass.preserveAttachmentCount;
		}
	}

	vk::RenderPassCreateInfo image_info = {};
	image_info.attachmentCount = (uint32_t)attachments.size();
	image_info.pAttachments    = attachments.data();
	image_info.subpassCount    = (uint32_t)subpasses.size();
	image_info.pSubpasses      = subpasses.data();
	image_info.dependencyCount = (uint32_t)dependencies.size();
	image_info.pDependencies   = dependencies.data();

	return std::shared_ptr<RenderPass>(new RenderPass(device.createRenderPass(image_info)));
}

VKDL_END