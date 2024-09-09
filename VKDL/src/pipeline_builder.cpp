#include "../include/vkdl/builder/pipeline_builder.h"

#include "../include/vkdl/core/context.h"

VKDL_BEGIN

PipelineBuilder::PipelineBuilder()
{
	clear();
}

void PipelineBuilder::clear()
{
	ss_info.clear();
	vib_desc.clear();
	via_desc.clear();
	viewports.clear();

	ia_info.topology               = vk::PrimitiveTopology::eTriangleList;
	ia_info.primitiveRestartEnable = false;

	raster_info.depthClampEnable        = false;
	raster_info.rasterizerDiscardEnable = false;
	raster_info.polygonMode             = vk::PolygonMode::eFill;
	raster_info.cullMode                = vk::CullModeFlagBits::eNone;
	raster_info.frontFace               = vk::FrontFace::eCounterClockwise;
	raster_info.depthBiasEnable         = false;
	raster_info.depthBiasConstantFactor = 0.f;
	raster_info.depthBiasClamp          = 0.f;
	raster_info.depthBiasSlopeFactor    = 0.f;
	raster_info.lineWidth               = 1.f;

	ms_info.rasterizationSamples  = vk::SampleCountFlagBits::e1;
	ms_info.sampleShadingEnable   = false;
	ms_info.minSampleShading      = 0.f;
	ms_info.pSampleMask           = nullptr;
	ms_info.alphaToCoverageEnable = false;
	ms_info.alphaToOneEnable      = false;

	depth_info.depthTestEnable       = false;
	depth_info.depthWriteEnable      = false;
	depth_info.depthCompareOp        = vk::CompareOp::eNever;
	depth_info.depthBoundsTestEnable = false;
	depth_info.stencilTestEnable     = false;
	depth_info.front                 = vk::StencilOpState();
	depth_info.back                  = vk::StencilOpState();
	depth_info.minDepthBounds        = 0.f;
	depth_info.maxDepthBounds        = 0.f;

	blend_info.logicOpEnable   = false;
	blend_info.logicOp         = vk::LogicOp::eClear;
	blend_info.attachmentCount = 0;
	blend_info.pAttachments    = nullptr;
	blend_info.blendConstants.fill(0.f);
}

PipelineBuilder& PipelineBuilder::addShaderStage(std::shared_ptr<ShaderModule>& module, const vk::PipelineShaderStageCreateInfo& image_info)
{
	modules.push_back(module);
	ss_info.push_back(image_info);
	return *this;
}

PipelineBuilder& PipelineBuilder::addVertexInput(uint32_t binding, uint32_t stride, vk::VertexInputRate input_rate)
{
	vib_desc.emplace_back(binding, stride, input_rate);
	return *this;
}

PipelineBuilder& PipelineBuilder::addVertexInputAtrribute(uint32_t binding, uint32_t location, vk::Format format, uint32_t offset)
{
	via_desc.emplace_back(location, binding, format, offset);
	return *this;
}

PipelineBuilder& PipelineBuilder::setPrimitiveTopology(vk::PrimitiveTopology topology, bool enable_restart)
{
	ia_info.topology               = topology;
	ia_info.primitiveRestartEnable = enable_restart;
	return *this;
}

PipelineBuilder& PipelineBuilder::addViewportScissor(vk::Viewport viewport, vk::Rect2D scissor)
{
	viewports.push_back(viewport);
	scissors.push_back(scissor);
	return *this;
}

PipelineBuilder& PipelineBuilder::enableRasterizerDepthClamp()
{
	raster_info.depthClampEnable = true;
	return *this;
}

PipelineBuilder& PipelineBuilder::enableRasterizerDiscard()
{
	raster_info.rasterizerDiscardEnable = true;
	return *this;
}

PipelineBuilder& PipelineBuilder::setRasterizerPolygonMode(vk::PolygonMode mode)
{
	raster_info.polygonMode = mode;
	return *this;
}

PipelineBuilder& PipelineBuilder::setRasterizerCullMode(vk::CullModeFlags flags)
{
	raster_info.cullMode = flags;
	return *this;
}

PipelineBuilder& PipelineBuilder::setRasterizerFrontFace(vk::FrontFace front_face)
{
	raster_info.frontFace = front_face;
	return *this;
}

PipelineBuilder& PipelineBuilder::setRasterizerDepthBias(float const_factor, float clamp, float slope_factor)
{
	raster_info.depthBiasEnable         = true;
	raster_info.depthBiasConstantFactor = const_factor;
	raster_info.depthBiasClamp          = clamp;
	raster_info.depthBiasSlopeFactor    = slope_factor;
	return *this;
}

PipelineBuilder& PipelineBuilder::setRasterizerLineWidth(float lineWidth)
{
	raster_info.lineWidth = lineWidth;
	return *this;
}

PipelineBuilder& PipelineBuilder::setRasterizationSamples(vk::SampleCountFlagBits flags)
{
	ms_info.rasterizationSamples = flags;
	return *this;
}

PipelineBuilder& PipelineBuilder::setSampleShading(float min_sample_shading)
{
	ms_info.sampleShadingEnable = true;
	ms_info.minSampleShading    = min_sample_shading;
	return *this;
}

PipelineBuilder& PipelineBuilder::enableAlphaToCoverage()
{
	ms_info.alphaToCoverageEnable = true;
	return *this;
}

PipelineBuilder& PipelineBuilder::enableAlphaToOne()
{
	ms_info.alphaToOneEnable = true;
	return *this;
}

PipelineBuilder& PipelineBuilder::enableDepthTest()
{
	depth_info.depthTestEnable = true;
	return *this;
}

PipelineBuilder& PipelineBuilder::enableDepthWrite()
{
	depth_info.depthWriteEnable = true;
	return *this;
}

PipelineBuilder& PipelineBuilder::setDepthCompareOp(vk::CompareOp op)
{
	depth_info.depthCompareOp = op;
	return *this;
}

PipelineBuilder& PipelineBuilder::enableDepthBoundsTest()
{
	depth_info.depthBoundsTestEnable = true;
	return *this;
}

PipelineBuilder& PipelineBuilder::enableStencilTest(vk::StencilOpState front, vk::StencilOpState back)
{
	depth_info.front = front;
	depth_info.back  = back;
	return *this;
}

PipelineBuilder& PipelineBuilder::setDepthBounds(float min, float max)
{
	depth_info.minDepthBounds = min;
	depth_info.maxDepthBounds = max;
	return *this;
}

PipelineBuilder& PipelineBuilder::enableBlendLogicOp()
{
	blend_info.logicOpEnable = true;
	return *this;
}

PipelineBuilder& PipelineBuilder::setBlendLogicOp(vk::LogicOp op)
{
	blend_info.logicOp = op;
	return *this;
}

PipelineBuilder& PipelineBuilder::setBlendConstants(float c0, float c1, float c2, float c3)
{
	blend_info.blendConstants[0] = c0;
	blend_info.blendConstants[1] = c1;
	blend_info.blendConstants[2] = c2;
	blend_info.blendConstants[3] = c3;
	return *this;
}

PipelineBuilder& PipelineBuilder::enableBlend()
{
	curr_blend_state.blendEnable;
	return *this;
}

PipelineBuilder& PipelineBuilder::setColorBlend(vk::BlendFactor src_factor, vk::BlendFactor dst_factor, vk::BlendOp op)
{
	curr_blend_state.srcColorBlendFactor = src_factor;
	curr_blend_state.dstColorBlendFactor = dst_factor;
	curr_blend_state.colorBlendOp        = op;
	return *this;
}

PipelineBuilder& PipelineBuilder::setAlphaBlend(vk::BlendFactor src_factor, vk::BlendFactor dst_factor, vk::BlendOp op)
{
	curr_blend_state.srcAlphaBlendFactor = src_factor;
	curr_blend_state.dstAlphaBlendFactor = dst_factor;
	curr_blend_state.alphaBlendOp        = op;
	return *this;
}

PipelineBuilder& PipelineBuilder::setColorWriteMask(bool r, bool g, bool b, bool a)
{
	auto& mask = curr_blend_state.colorWriteMask = {};
	if (r) mask |= vk::ColorComponentFlags(vk::ColorComponentFlagBits::eR);
	if (g) mask |= vk::ColorComponentFlags(vk::ColorComponentFlagBits::eG);
	if (b) mask |= vk::ColorComponentFlags(vk::ColorComponentFlagBits::eB);
	if (a) mask |= vk::ColorComponentFlags(vk::ColorComponentFlagBits::eA);

	return *this;
}

PipelineBuilder& PipelineBuilder::pushCurrentColorBlendAttachmentState()
{
	blend_states.push_back(curr_blend_state);
	curr_blend_state = vk::PipelineColorBlendAttachmentState();
	return *this;
}

PipelineBuilder& PipelineBuilder::addDynamicState(vk::DynamicState state)
{
	dynamic_states.push_back(state);
	return *this;
}

PipelineBuilder& PipelineBuilder::setPipelineLayout(std::shared_ptr<PipelineLayout> layout)
{
	this->layout = layout;
	return *this;
}

PipelineBuilder& PipelineBuilder::setRenderPass(std::shared_ptr<RenderPass> renderpass)
{
	this->renderpass = renderpass;
	return *this;
}

std::shared_ptr<Pipeline> PipelineBuilder::build()
{
	auto& ctx    = Context::get();
	auto& device = ctx.device;

	vk::PipelineVertexInputStateCreateInfo vertex_info = {};
	vertex_info.vertexBindingDescriptionCount   = (uint32_t)vib_desc.size();
	vertex_info.pVertexBindingDescriptions      = vib_desc.data();
	vertex_info.vertexAttributeDescriptionCount = (uint32_t)via_desc.size();
	vertex_info.pVertexAttributeDescriptions    = via_desc.data();

	vk::PipelineTessellationStateCreateInfo tesselation_info = {};

	vk::PipelineViewportStateCreateInfo viewport_info = {};
	if (viewports.empty()) {
		viewport_info.viewportCount = 1;
		viewport_info.pViewports    = nullptr;
		viewport_info.scissorCount  = 1;
		viewport_info.pScissors     = nullptr;
	} else {
		viewport_info.viewportCount = (uint32_t)viewports.size();
		viewport_info.pViewports    = viewports.data();
		viewport_info.scissorCount  = (uint32_t)scissors.size();
		viewport_info.pScissors     = scissors.data();
	}

	blend_info.attachmentCount = (uint32_t)blend_states.size();
	blend_info.pAttachments    = blend_states.data();

	vk::PipelineDynamicStateCreateInfo ds_info = {};
	ds_info.dynamicStateCount = (uint32_t)dynamic_states.size();
	ds_info.pDynamicStates    = dynamic_states.data();

	vk::GraphicsPipelineCreateInfo image_info = {};
	image_info.stageCount          = (uint32_t)ss_info.size();
	image_info.pStages             = ss_info.data();
	image_info.pVertexInputState   = &vertex_info;
	image_info.pInputAssemblyState = &ia_info;
	image_info.pTessellationState  = &tesselation_info;
	image_info.pViewportState      = &viewport_info;
	image_info.pRasterizationState = &raster_info;
	image_info.pMultisampleState   = &ms_info;
	image_info.pDepthStencilState  = &depth_info;
	image_info.pColorBlendState    = &blend_info;
	image_info.pDynamicState       = &ds_info;
	image_info.layout              = *layout;
	image_info.renderPass          = renderpass->get();
	image_info.subpass             = 0;
	image_info.basePipelineHandle  = nullptr;
	image_info.basePipelineIndex   = 0;

	auto result = device.createGraphicsPipeline(ctx.pipeline_cache, image_info);
	VKDL_CHECK_MSG(result.result == vk::Result::eSuccess, "Failed to create graphics pipeline");

	return std::shared_ptr<Pipeline>(new Pipeline(result.value, layout, modules));
}

VKDL_END
