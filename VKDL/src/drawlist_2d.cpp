#include "../include/vkdl/graphics/drawlist_2d.h"

#include "../include/vkdl/core/context.h"
#include "../include/vkdl/core/render_target.h"
#include "../include/vkdl/core/render_states.h"
#include "../include/vkdl/core/builtin_objects.h"
#include "../include/vkdl/graphics/texture.h"

VKDL_BEGIN

DrawCommand2D::DrawCommand2D() :
	vertex_offset(0),
	vertex_count(0),
	index_offset(0),
	index_count(0)
{
}

DrawList2D::DrawList2D() :
	Drawable(VKDL_BUILTIN_RENDERPASS0_UUID, VKDL_BUILTIN_PIPELINE0_UUID),
	vertex_buffer(vk::BufferUsageFlagBits::eVertexBuffer, vk::MemoryPropertyFlagBits::eHostVisible),
	index_buffer(vk::BufferUsageFlagBits::eIndexBuffer, vk::MemoryPropertyFlagBits::eHostVisible),
	update_buffer(false)
{
	registerBuiltinPipeline(VKDL_BUILTIN_PIPELINE0_UUID);
	registerBuiltinPipeline(VKDL_BUILTIN_PIPELINE1_UUID);

	auto& cmd = commands.emplace_back();
}

void DrawList2D::addRawTriangle(const Vertex2D& v0, const Vertex2D& v1, const Vertex2D& v2)
{
	auto idx = reservePrimitives(3, 3);

	vertices.emplace_back(v0);
	vertices.emplace_back(v1);
	vertices.emplace_back(v2);

	indices.emplace_back(idx + 0);
	indices.emplace_back(idx + 1);
	indices.emplace_back(idx + 2);
}

void DrawList2D::addFilledTriangle(const vec2& p0, const vec2& p1, const vec2& p2, const Color& col)
{
	auto idx = reservePrimitives(3, 3);

	vertices.emplace_back(p0, col);
	vertices.emplace_back(p1, col);
	vertices.emplace_back(p2, col);

	indices.emplace_back(idx + 0);
	indices.emplace_back(idx + 1);
	indices.emplace_back(idx + 2);
}

void DrawList2D::addImage(const Texture& texture, const vec2& pos, const vec2& size, const vec2& uv0, const vec2& uv1, const Color& col)
{
	pushTexture(texture);
	addImage(pos, size, uv0, uv1, col);
	popTexture();
}

void DrawList2D::addImage(const vec2& pos, const vec2& size, const vec2& uv0, const vec2& uv1, const Color& col)
{
	auto idx = reservePrimitives(4, 6);

	vertices.emplace_back(vec2{pos.x, pos.y}, vec2{uv0.x, uv0.y}, col);
	vertices.emplace_back(vec2{pos.x + size.x, pos.y}, vec2{uv1.x, uv0.y}, col);
	vertices.emplace_back(vec2{pos.x + size.x, pos.y + size.y}, vec2{uv1.x, uv1.y}, col);
	vertices.emplace_back(vec2{pos.x, pos.y + size.y}, vec2{uv0.x, uv1.y}, col);

	indices.emplace_back(idx + 0);
	indices.emplace_back(idx + 1);
	indices.emplace_back(idx + 2);
	indices.emplace_back(idx + 0);
	indices.emplace_back(idx + 2);
	indices.emplace_back(idx + 3);
}

void DrawList2D::addImageQuad(const vec2& p0, const vec2& p1, const vec2& p2, const vec2& p3, const vec2& uv0, const vec2& uv1, const vec2& uv2, const vec2& uv3, const Color& col)
{
	auto idx = reservePrimitives(4, 6);

	vertices.emplace_back(p0, uv0, col);
	vertices.emplace_back(p1, uv1, col);
	vertices.emplace_back(p2, uv2, col);
	vertices.emplace_back(p3, uv3, col);

	indices.emplace_back(idx + 0);
	indices.emplace_back(idx + 1);
	indices.emplace_back(idx + 2);
	indices.emplace_back(idx + 0);
	indices.emplace_back(idx + 2);
	indices.emplace_back(idx + 3);
}

void DrawList2D::pushTexture(const Texture& texture)
{
	texture_stack.push_back(&texture);
	newCommand();
}

void DrawList2D::popTexture()
{
	texture_stack.pop_back();
	newCommand();
}

void DrawList2D::pushClipRectFullScreen()
{
	clip_rect_stack.emplace_back();
	newCommand();
}

void DrawList2D::pushClipRect(vk::Rect2D rect, bool intersect)
{
	struct Rect {
		Rect(const vk::Rect2D& rect) :
			x_min(rect.offset.x),
			y_min(rect.offset.y),
			x_max(x_min + rect.extent.width),
			y_max(y_min + rect.extent.height)
		{
		}

		int32_t x_min;
		int32_t y_min;
		int32_t x_max;
		int32_t y_max;
	};

	if (intersect) {
		Rect new_rect(rect);
		Rect curr_rect(clip_rect_stack.back());
		
		new_rect.x_min = std::max(new_rect.x_min, curr_rect.x_min);
		new_rect.y_min = std::max(new_rect.y_min, curr_rect.y_min);
		new_rect.x_max = std::min(new_rect.x_max, curr_rect.x_max);
		new_rect.y_max = std::min(new_rect.y_max, curr_rect.y_max);

		rect.offset.x      = new_rect.x_min;
		rect.offset.y      = new_rect.y_min;
		rect.extent.width  = new_rect.x_max - new_rect.x_min;
		rect.extent.height = new_rect.y_max - new_rect.y_min;
	}

	clip_rect_stack.push_back(rect);
	newCommand();
}

void DrawList2D::pushClipRect(int32_t off_x, int32_t off_y, uint32_t width, uint32_t height, bool intersect)
{
	pushClipRect({vk::Offset2D{off_x, off_y}, vk::Extent2D{width, height}}, intersect);
}

void DrawList2D::popClipRect()
{
	clip_rect_stack.pop_back();
	newCommand();
}

void DrawList2D::pushTransform(const Transform2D& transform)
{
	transform_stack.push_back(transform);
	newCommand();
}

void DrawList2D::popTransform()
{
	transform_stack.pop_back();
	newCommand();
}

void DrawList2D::clear()
{
	commands.clear();
	vertices.clear();
	indices.clear();

	texture_stack.clear();
	clip_rect_stack.clear();
	transform_stack.clear();

	vertex_buffer.clear();
	index_buffer.clear();
	update_buffer = false;

	commands.emplace_back();
}

void DrawList2D::draw(RenderTarget& target, RenderStates& states, const RenderOptions& options) const
{
	if (commands.empty()) return;

	if (std::exchange(update_buffer, false)) {
		vertex_buffer.resize(vertices.size());
		index_buffer.resize(indices.size());
		memcpy(vertex_buffer.map(), vertices.data(), vertex_buffer.size_in_byte());
		memcpy(index_buffer.map(), indices.data(), index_buffer.size_in_byte());
		vertex_buffer.flush();
		index_buffer.flush();
	}

	auto& ctx    = Context::get();
	auto cmd     = target.getCommandBuffer();
	auto offset  = vk::DeviceSize{ 0 };
	auto fb_size = target.getFrameBufferSize();

	auto& pipeline_layout = ctx.getPipeline(getPipelineUUID()).getPipelineLayout();
	
	auto transform = Transform2D()
		.translate(-1.f, -1.f)
		.scale(2.f / fb_size.x, 2.f / fb_size.y);

	states.updateRenderPassUUID(getRenderPassUUID());

	cmd.bindVertexBuffers(0, 1, &vertex_buffer.getBuffer(), &offset);
	cmd.bindIndexBuffer(index_buffer.getBuffer(), 0, vk::IndexType::eUint32);
	
	for (const auto& command : commands) {
		if (command.descriptor_set != nullptr) {
			states.updatePipelineUUID(VKDL_BUILTIN_PIPELINE0_UUID);
			
			cmd.bindDescriptorSets(
				vk::PipelineBindPoint::eGraphics,
				pipeline_layout,
				0,
				1, &command.descriptor_set,
				0, nullptr);
		} else {
			states.updatePipelineUUID(VKDL_BUILTIN_PIPELINE1_UUID);
		}

		if (command.clip_rect == vk::Rect2D())
			states.updateScissor({{0, 0}, {fb_size.x, fb_size.y}});
		else
			states.updateScissor(command.clip_rect);

		auto new_transform = transform * command.transform;

		cmd.pushConstants(
			pipeline_layout, 
			vk::ShaderStageFlagBits::eVertex, 
			0, 
			sizeof(Transform2D), 
			&new_transform);
		
		states.bind(target, options);

		cmd.drawIndexed(command.index_count, 1, command.index_offset, command.vertex_offset, 0);
	}
}

void DrawList2D::newCommand()
{
	auto* cmd = &commands.back();

	if (cmd->index_count != 0)
		cmd = &commands.emplace_back();

	cmd->descriptor_set = texture_stack.empty() ? nullptr : texture_stack.back()->getDescriptorSet();
	cmd->transform      = transform_stack.empty() ? Transform2D() : transform_stack.back();
	cmd->vertex_offset  = (uint32_t)vertices.size();
	cmd->index_offset   = (uint32_t)indices.size();
}

uint32_t DrawList2D::reservePrimitives(uint32_t vert_size, uint32_t idx_size)
{
	auto& cmd = commands.back();
	cmd.vertex_count += vert_size;
	cmd.index_count  += idx_size;

	vertices.reserve(vertices.size() + vert_size);
	indices.reserve(indices.size() + idx_size);

	update_buffer = true;

	return (uint32_t)vertices.size();
}

VKDL_END