#include "../include/vkdl/graphics/texture_view.h"

#include "../include/vkdl/core/render_target.h"
#include "../include/vkdl/core/render_states.h"
#include "../include/vkdl/core/builtin_objects.h"

VKDL_BEGIN

TextureView::TextureView() :
	Offset(),
	Size(),
	FilledColor(Colors::White)
{
	registerBuiltinPipeline(VKDL_BUILTIN_PIPELINE2_UUID);
}

TextureView::TextureView(const Texture& texture, const vec2& offset, const vec2& size) :
	TexturePtr(&texture),
	Offset(offset),
	Size(size),
	FilledColor(Colors::White)
{
	registerBuiltinPipeline(VKDL_BUILTIN_PIPELINE2_UUID);
}

bool TextureView::empty() const
{
	return TexturePtr == nullptr;
}

void TextureView::draw(RenderTarget& target, RenderStates& states, const RenderOptions& options) const
{
	if (!TexturePtr) return;

	auto& ctx    = Context::get();
	auto cmd     = target.getCommandBuffer();
	auto fb_size = target.getFrameBufferSize();

	auto& pipeline_layout = ctx.getPipeline(VKDL_BUILTIN_PIPELINE2_UUID).getPipelineLayout();
	auto& texture         = *TexturePtr;

	states.updateRenderPassUUID(VKDL_BUILTIN_RENDERPASS0_UUID);
	states.updatePipelineUUID(VKDL_BUILTIN_PIPELINE2_UUID);

	cmd.bindDescriptorSets(
		vk::PipelineBindPoint::eGraphics,
		pipeline_layout,
		0,
		1, &texture.getDescriptorSet(),
		0, nullptr);

	struct {
		Transform2D transform;
		vec4        color;
		vec2        vertex[4];
		vec2        uv[4];
	} pc;

	vec2 extent = texture.extent();
	vec2 size   = Size;
	vec2 offset = Offset;

	pc.transform = Transform2D().translate(-1.f, -1.f).scale(1.f / fb_size.x, 1.f / fb_size.y) * getTransform();
	pc.color     = FilledColor.get().to_vec4();
	pc.vertex[0] = vec2(0, 0);
	pc.vertex[1] = vec2(size.x, 0);
	pc.vertex[2] = vec2(size.x, size.y);
	pc.vertex[3] = vec2(0, size.y);
	pc.uv[0]     = vec2(offset.x / extent.x, offset.y / extent.y);
	pc.uv[1]     = vec2((offset.x + size.x) / extent.x, offset.y / extent.y);
	pc.uv[2]     = vec2((offset.x + size.x) / extent.x, (offset.y + size.y) / extent.y);
	pc.uv[3]     = vec2(offset.x / extent.x, (offset.y + size.y) / extent.y);

	cmd.pushConstants(
		pipeline_layout,
		vk::ShaderStageFlagBits::eVertex,
		0,
		sizeof(pc),
		&pc);

	states.updateScissor({ {0, 0}, {fb_size.x, fb_size.y} });
	states.bind(target, options);

	cmd.draw(6, 1, 0, 0);
}

VKDL_END