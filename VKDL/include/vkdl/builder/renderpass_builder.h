#pragma once

#include "../core/renderpass.h"

VKDL_BEGIN

class RenderPassBuilder
{
public:
	void clear();

	RenderPassBuilder& setAttachmentFormat(vk::Format format);
	RenderPassBuilder& setAttachmentSampleCount(vk::SampleCountFlagBits samples);
	RenderPassBuilder& setAttachmentLoadOp(vk::AttachmentLoadOp op);
	RenderPassBuilder& setAttachmentStoreOp(vk::AttachmentStoreOp op);
	RenderPassBuilder& setAttachmentStencilLoadOp(vk::AttachmentLoadOp op);
	RenderPassBuilder& setAttachmentStencilStoreOp(vk::AttachmentStoreOp op);
	RenderPassBuilder& setAttachmentInitialLayout(vk::ImageLayout layout);
	RenderPassBuilder& setAttachmentFinalLayout(vk::ImageLayout layout);
	RenderPassBuilder& pushCurrentAttachment();

	RenderPassBuilder& setSubpassPipelineBindPoint(vk::PipelineBindPoint bind_point);
	RenderPassBuilder& addSubpassInputAttachment(uint32_t attachment, vk::ImageLayout layout);
	RenderPassBuilder& addSubpassColorAttachment(uint32_t attachment, vk::ImageLayout layout);
	RenderPassBuilder& addSubpassResolveAttachment(uint32_t attachment, vk::ImageLayout layout);
	RenderPassBuilder& setSubpassDepthStencilAttachment(uint32_t attachment, vk::ImageLayout layout);
	RenderPassBuilder& addSubpassPreserveAttachment(uint32_t attachment);
	RenderPassBuilder& pushCurrentSubpass();

	RenderPassBuilder& setDependencySrc(uint32_t subpass, vk::PipelineStageFlags stage, vk::AccessFlags access_mask);
	RenderPassBuilder& setDependencyDst(uint32_t subpass, vk::PipelineStageFlags stage, vk::AccessFlags access_mask);
	RenderPassBuilder& setDependencyFlags(vk::DependencyFlagBits flags);
	RenderPassBuilder& pushCurrentDependency();

	std::shared_ptr<RenderPass> build();

private:
	std::vector<vk::AttachmentDescription> attachments;
	std::vector<vk::SubpassDescription>    subpasses;
	std::vector<vk::AttachmentReference>   input_attachments;
	std::vector<vk::AttachmentReference>   color_attachments;
	std::vector<vk::AttachmentReference>   resolve_attachments;
	std::vector<vk::AttachmentReference>   depth_stencil_attachments;
	std::vector<uint32_t>                  preserve_attachments;
	std::vector<vk::SubpassDependency>     dependencies;

	vk::AttachmentDescription curr_attachment;
	vk::SubpassDescription    curr_subpass;
	vk::SubpassDependency     curr_dependency;
};

VKDL_END