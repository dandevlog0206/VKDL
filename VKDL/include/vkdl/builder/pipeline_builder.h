#pragma once

#include "../core/pipeline.h"
#include "../core/shader_module.h"
#include "../core/renderpass.h"

VKDL_BEGIN

class PipelineBuilder
{
public:
	PipelineBuilder();

	void clear();

	PipelineBuilder& addShaderStage(std::shared_ptr<ShaderModule>& module, const vk::PipelineShaderStageCreateInfo& image_info);

	PipelineBuilder& addVertexInput(uint32_t binding, uint32_t stride, vk::VertexInputRate input_rate = vk::VertexInputRate::eVertex);
	PipelineBuilder& addVertexInputAtrribute(uint32_t binding, uint32_t location, vk::Format format, uint32_t offset = 0);

	PipelineBuilder& setPrimitiveTopology(vk::PrimitiveTopology topology, bool enable_restart = false);
	
	PipelineBuilder& addViewportScissor(vk::Viewport viewport, vk::Rect2D scissor);
	
	PipelineBuilder& enableRasterizerDepthClamp();
	PipelineBuilder& enableRasterizerDiscard();
	PipelineBuilder& setRasterizerPolygonMode(vk::PolygonMode mode);
	PipelineBuilder& setRasterizerCullMode(vk::CullModeFlags flags);
	PipelineBuilder& setRasterizerFrontFace(vk::FrontFace front_face);
	PipelineBuilder& setRasterizerDepthBias(float const_factor, float clamp, float slope_factor);
	PipelineBuilder& setRasterizerLineWidth(float lineWidth);

	PipelineBuilder& setRasterizationSamples(vk::SampleCountFlagBits flags);
	PipelineBuilder& setSampleShading(float min_sample_shading);
	PipelineBuilder& enableAlphaToCoverage();
	PipelineBuilder& enableAlphaToOne();
	
	PipelineBuilder& enableDepthTest();
	PipelineBuilder& enableDepthWrite();
	PipelineBuilder& setDepthCompareOp(vk::CompareOp op);
	PipelineBuilder& enableDepthBoundsTest();
	PipelineBuilder& enableStencilTest(vk::StencilOpState front, vk::StencilOpState back);
	PipelineBuilder& setDepthBounds(float min, float max);

	PipelineBuilder& enableBlendLogicOp();
	PipelineBuilder& setBlendLogicOp(vk::LogicOp op);
	PipelineBuilder& setBlendConstants(float c0, float c1, float c2, float c3);

	PipelineBuilder& enableBlend();
	PipelineBuilder& setColorBlend(vk::BlendFactor src_factor, vk::BlendFactor dst_factor, vk::BlendOp op);
	PipelineBuilder& setAlphaBlend(vk::BlendFactor src_factor, vk::BlendFactor dst_factor, vk::BlendOp op);
	PipelineBuilder& setColorWriteMask(bool r, bool g, bool b, bool a);
	PipelineBuilder& pushCurrentColorBlendAttachmentState();

	PipelineBuilder& addDynamicState(vk::DynamicState state);

	PipelineBuilder& setPipelineLayout(std::shared_ptr<PipelineLayout> layout);
	PipelineBuilder& setRenderPass(std::shared_ptr<RenderPass> renderpass);

	std::shared_ptr<Pipeline> build();

private:
	std::vector<vk::PipelineShaderStageCreateInfo>     ss_info;
	std::vector<vk::VertexInputBindingDescription>     vib_desc;
	std::vector<vk::VertexInputAttributeDescription>   via_desc;
	std::vector<vk::Viewport>                          viewports;
	std::vector<vk::Rect2D>                            scissors;
	std::vector<vk::DynamicState>                      dynamic_states;
	std::vector<vk::PipelineColorBlendAttachmentState> blend_states;
	std::vector<std::shared_ptr<ShaderModule>>         modules;

	vk::PipelineInputAssemblyStateCreateInfo ia_info; 
	vk::PipelineRasterizationStateCreateInfo raster_info;
	vk::PipelineMultisampleStateCreateInfo   ms_info;
	vk::PipelineDepthStencilStateCreateInfo  depth_info;
	vk::PipelineColorBlendStateCreateInfo    blend_info;
	
	std::shared_ptr<PipelineLayout> layout;
	std::shared_ptr<RenderPass>     renderpass;

	vk::PipelineColorBlendAttachmentState curr_blend_state;
};

VKDL_END