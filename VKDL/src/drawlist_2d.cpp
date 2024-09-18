#include "../include/vkdl/graphics/drawlist_2d.h"

#include "../include/vkdl/core/context.h"
#include "../include/vkdl/core/builtin_objects.h"
#include "../include/vkdl/graphics/texture.h"
#include "../include/vkdl/graphics/font.h"

VKDL_BEGIN

DrawCommand2D::DrawCommand2D() :
	texture(nullptr),
	vertex_offset(0),
	vertex_count(0),
	index_offset(0),
	index_count(0)
{
}

DrawList2D::DrawList2D() :
	vertex_buffer(Buffer<Vertex2D>::createVertexBuffer(0)),
	index_buffer(Buffer<uint32_t>::createIndexBuffer(0)),
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

void DrawList2D::addRawQuad(const Vertex2D& v0, const Vertex2D& v1, const Vertex2D& v2, const Vertex2D& v3)
{
	auto idx = reservePrimitives(4, 6);

	vertices.emplace_back(v0);
	vertices.emplace_back(v1);
	vertices.emplace_back(v2);
	vertices.emplace_back(v3);

	indices.emplace_back(idx + 0);
	indices.emplace_back(idx + 1);
	indices.emplace_back(idx + 2);
	indices.emplace_back(idx + 2);
	indices.emplace_back(idx + 3);
	indices.emplace_back(idx + 0);
}

void DrawList2D::addDot(const vec2& p, float r, const Color& col)
{
	r = 0.5f * std::abs(r);

	auto idx = reservePrimitives(4, 6);

	vertices.emplace_back(p + vec2(-r, -r), col);
	vertices.emplace_back(p + vec2(+r, -r), col);
	vertices.emplace_back(p + vec2(+r, +r), col);
	vertices.emplace_back(p + vec2(-r, +r), col);

	indices.emplace_back(idx + 0);
	indices.emplace_back(idx + 1);
	indices.emplace_back(idx + 2);
	indices.emplace_back(idx + 2);
	indices.emplace_back(idx + 3);
	indices.emplace_back(idx + 0);
}

void DrawList2D::addLine(const vec2& p0, const vec2& p1, float width, const Color& col)
{
	auto idx = reservePrimitives(4, 6);

	auto v0 = glm::normalize(p1 - p0);
	auto v1 = vec2(-v0.y, v0.x);

	vertices.emplace_back(p0 + v1 * width - v0 * width, col);
	vertices.emplace_back(p0 - v1 * width - v0 * width, col);
	vertices.emplace_back(p1 + v1 * width + v0 * width, col);
	vertices.emplace_back(p1 - v1 * width + v0 * width, col);

	indices.emplace_back(idx + 0);
	indices.emplace_back(idx + 2);
	indices.emplace_back(idx + 1);
	indices.emplace_back(idx + 1);
	indices.emplace_back(idx + 2);
	indices.emplace_back(idx + 3);
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

void DrawList2D::addFilledCircleFan(const vec2& pos, float radius, float theta_min, float theta_max, const Color& col, uint32_t seg_count)
{
	auto idx = reservePrimitives(seg_count + 1, 3 * seg_count);

	vertices.emplace_back(pos, col);
	for (uint32_t i = 0; i <= seg_count; ++i) {
		float theta = (theta_max - theta_min) * i / seg_count + theta_min;
		vertices.emplace_back(pos + radius * vec2(cosf(theta), sinf(theta)), col);
	}

	for (uint32_t i = 1; i <= seg_count; ++i) {
		indices.emplace_back(idx);
		indices.emplace_back(idx + i);
		indices.emplace_back(idx + i + 1);
	}
}

void DrawList2D::addQuad(const vec2& p0, const vec2& p1, const vec2& p2, const vec2& p3, const Color& col)
{
	auto idx = reservePrimitives(4, 6);

	vertices.emplace_back(p0, col);
	vertices.emplace_back(p1, col);
	vertices.emplace_back(p2, col);
	vertices.emplace_back(p3, col);

	indices.emplace_back(idx + 0);
	indices.emplace_back(idx + 1);
	indices.emplace_back(idx + 2);
	indices.emplace_back(idx + 2);
	indices.emplace_back(idx + 3);
	indices.emplace_back(idx + 0);
}

void DrawList2D::addFilledRect(const vec2& pos, const vec2& size, const Color& col)
{
	auto idx = reservePrimitives(4, 6);

	vertices.emplace_back(pos, col);
	vertices.emplace_back(pos + vec2(size.x, 0), col);
	vertices.emplace_back(pos + size, col);
	vertices.emplace_back(pos + vec2(0, size.y), col);

	indices.emplace_back(idx + 0);
	indices.emplace_back(idx + 1);
	indices.emplace_back(idx + 2);
	indices.emplace_back(idx + 2);
	indices.emplace_back(idx + 3);
	indices.emplace_back(idx + 0);
}

void DrawList2D::addFilledRoundRect(const vec2& pos, const vec2& size, const vec4& radius, const Color& col)
{
	addFilledCircleFan(pos + vec2(radius[0]), radius[0], to_radian(180), to_radian(270), col);
	addFilledCircleFan(pos + vec2(size.x - radius[1], radius[1]), radius[1], to_radian(270), to_radian(360), col);
	addFilledCircleFan(pos + size - vec2(radius[2]), radius[2], to_radian(0), to_radian(90), col);
	addFilledCircleFan(pos + vec2(radius[3], size.y - radius[3]), radius[3], to_radian(90), to_radian(180), col);

	addFilledRect(pos + vec2(radius[0], 0), vec2(size.x - radius[0] - radius[1], radius[0]), col);
	addFilledRect(pos + vec2(size.x - radius[1], radius[1]), vec2(radius[1], size.y - radius[1] - radius[2]), col);
	addFilledRect(pos + vec2(radius[3], size.y - radius[2]), vec2(size.x - radius[2] - radius[3], radius[2]), col);
	addFilledRect(pos + vec2(0, radius[0]), vec2(radius[3], size.y - radius[0] - radius[3]), col);

	addFilledRect(pos + vec2(radius[3], radius[0]), size - vec2(radius[1] + radius[3], radius[0] + radius[2]), col);
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

void DrawList2D::addText(const vec2& pos, const std::string& text, const TextStyle& style)
{
	pushTexture(style.font->getTexture(style.character_size));

	if (text.empty()) return;

	size_t vertex_begin = vertices.size();

	const float italicShear         = (style.italic) ? to_radian(12.f) : (radian)0.f;
	const float underlineOffset     = style.font->getUnderlinePosition(style.character_size);
	const float underlineThickness  = style.font->getUnderlineThickness(style.character_size);
	const float strikeThroughOffset = style.font->getGlyph(U'x', style.character_size, style.bold).bounds.center().y;

	float       whitespaceWidth = style.font->getGlyph(U' ', style.character_size, style.bold).advance;
	const float letterSpacing   = (whitespaceWidth / 3.f) * (style.letter_spacing_factor - 1.f);
	whitespaceWidth += letterSpacing;
	const float lineSpacing = style.font->getLineSpacing(style.character_size) * style.line_spacing_factor;

	float    x         = 0.f;
	auto     y         = static_cast<float>(style.character_size);

	auto     min_x     = static_cast<float>(style.character_size);
	auto     min_y     = static_cast<float>(style.character_size);
	float    max_x     = 0.f;
	float    max_y     = 0.f;
	uint32_t prevChar = 0;

	for (const uint32_t curChar : text) {
		if (curChar == U'\r') continue;

		x += style.font->getKerning(prevChar, curChar, style.character_size, style.bold);

		if (style.underline && (curChar == U'\n' && prevChar != U'\n'))
			addTextLine(x, y, style.fill_color, underlineOffset, underlineThickness);

		if (style.strike_through && (curChar == U'\n' && prevChar != U'\n'))
			addTextLine(x, y, style.fill_color, strikeThroughOffset, underlineThickness);

		prevChar = curChar;

		if ((curChar == U' ') || (curChar == U'\n') || (curChar == U'\t')) {
			min_x = std::min(min_x, x);
			min_y = std::min(min_y, y);

			switch (curChar) {
				case U' ':  x += whitespaceWidth; break;
				case U'\t': x += whitespaceWidth * 4; break;
				case U'\n': y += lineSpacing; x = 0; break;
			}

			max_x = std::max(max_x, x);
			max_y = std::max(max_y, y);

			continue;
		}

		const Glyph& glyph = style.font->getGlyph(curChar, style.character_size, style.bold);

		addGlyphQuad(vec2(x, y), style.fill_color, glyph, italicShear);

		const vec2 p1 = glyph.bounds.position;
		const vec2 p2 = glyph.bounds.position + glyph.bounds.size;

		min_x = std::min(min_x, x + p1.x - italicShear * p2.y);
		max_x = std::max(max_x, x + p2.x - italicShear * p1.y);
		min_y = std::min(min_y, y + p1.y);
		max_y = std::max(max_y, y + p2.y);

		x += glyph.advance + letterSpacing;
	}

	if (style.underline && (x > 0))
		addTextLine(x, y, style.fill_color, underlineOffset, underlineThickness);

	if (style.strike_through && (x > 0))
		addTextLine(x, y, style.fill_color, strikeThroughOffset, underlineThickness);

	vec2 align((min_x - max_x) * style.align_h - min_x, (min_y - max_y) * style.align_v - min_y);

	for (size_t i = vertex_begin; i < vertices.size(); ++i)
		vertices[i].pos += pos + align;

	popTexture();
}

void DrawList2D::addGlyphQuad(const vec2& pos, const Color& color, const Glyph& glyph, float italicShear)
{
	const vec2 padding(1.f, 1.f);

	const vec2 p1 = glyph.bounds.position - padding;
	const vec2 p2 = glyph.bounds.position + glyph.bounds.size + padding;

	const auto uv1 = vec2(glyph.texture_rect.position) - padding;
	const auto uv2 = vec2(glyph.texture_rect.position + glyph.texture_rect.size) + padding;

	auto idx = reservePrimitives(4, 6);

	vertices.emplace_back(pos + vec2(p1.x - italicShear * p1.y, p1.y), vec2{uv1.x, uv1.y}, color);
	vertices.emplace_back(pos + vec2(p2.x - italicShear * p1.y, p1.y), vec2{uv2.x, uv1.y}, color);
	vertices.emplace_back(pos + vec2(p1.x - italicShear * p2.y, p2.y), vec2{uv1.x, uv2.y}, color);
	vertices.emplace_back(pos + vec2(p2.x - italicShear * p2.y, p2.y), vec2{uv2.x, uv2.y}, color);

	indices.emplace_back(idx + 0);
	indices.emplace_back(idx + 1);
	indices.emplace_back(idx + 2);
	indices.emplace_back(idx + 2);
	indices.emplace_back(idx + 1);
	indices.emplace_back(idx + 3);
}

void DrawList2D::addTextLine(float lineLength, float lineTop, const Color& color, float offset, float thickness, float outlineThickness)
{
	const float top    = std::floor(lineTop + offset - (thickness / 2) + 0.5f);
	const float bottom = top + std::floor(thickness + 0.5f);

	auto idx = reservePrimitives(4, 6);

	vertices.emplace_back(vec2{-outlineThickness, top - outlineThickness}, color);
	vertices.emplace_back(vec2{lineLength + outlineThickness, top - outlineThickness}, color);
	vertices.emplace_back(vec2{-outlineThickness, bottom + outlineThickness}, color);
	vertices.emplace_back(vec2{lineLength + outlineThickness, bottom + outlineThickness}, color);

	indices.emplace_back(idx + 0);
	indices.emplace_back(idx + 1);
	indices.emplace_back(idx + 2);
	indices.emplace_back(idx + 2);
	indices.emplace_back(idx + 1);
	indices.emplace_back(idx + 3);
}

void DrawList2D::pushTexture(const Texture& texture)
{
	bool new_cmd = true;
	if (!commands.empty() && commands.back().texture == &texture) new_cmd = false;
	texture_stack.push_back(&texture);

	if (new_cmd) newCommand();
}

void DrawList2D::popTexture()
{
	auto* texture = texture_stack.back();
	texture_stack.pop_back();

	if (!commands.empty() && commands.back().texture == texture) return;

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

	update_buffer = true;

	commands.emplace_back();
}

void DrawList2D::draw(RenderTarget& target, RenderStates& states, const RenderOptions& options) const
{
	if (commands.empty()) return;
	if (commands.front().index_count == 0) return;

	if (std::exchange(update_buffer, false)) {
		vertex_buffer.resize(vertices.size(), false);
		index_buffer.resize(indices.size(), false);
		memcpy(vertex_buffer.map(), vertices.data(), vertex_buffer.size_in_bytes());
		memcpy(index_buffer.map(), indices.data(), index_buffer.size_in_bytes());
		vertex_buffer.flush();
		index_buffer.flush();
	}

	auto& ctx    = Context::get();
	auto cmd     = target.getCommandBuffer();
	auto offset  = vk::DeviceSize{ 0 };
	auto fb_size = target.getFrameBufferSize();

	auto& pipeline_layout = ctx.getPipeline(VKDL_BUILTIN_PIPELINE0_UUID).getPipelineLayout();
	
	auto transform = Transform2D()
		.translate(-1.f, -1.f)
		.scale(2.f / fb_size.x, 2.f / fb_size.y);

	states.updateRenderPassUUID(VKDL_BUILTIN_RENDERPASS0_UUID);

	cmd.bindVertexBuffers(0, 1, &vertex_buffer.getBuffer(), &offset);
	cmd.bindIndexBuffer(index_buffer.getBuffer(), 0, vk::IndexType::eUint32);
	
	for (const auto& command : commands) {
		if (command.texture != nullptr) {
			states.updatePipelineUUID(VKDL_BUILTIN_PIPELINE0_UUID);
			
			cmd.bindDescriptorSets(
				vk::PipelineBindPoint::eGraphics,
				pipeline_layout,
				0,
				1, &command.texture->getDescriptorSet(),
				0, nullptr);

			auto texture_size = (vec2)command.texture->extent();
			cmd.pushConstants(
				pipeline_layout,
				vk::ShaderStageFlagBits::eVertex,
				sizeof(Transform2D),
				sizeof(vec2),
				&texture_size);
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

		cmd.drawIndexed(command.index_count, 1, command.index_offset, 0, 0);
	}
}

void DrawList2D::newCommand()
{
	auto* cmd = &commands.back();

	if (cmd->index_count != 0)
		cmd = &commands.emplace_back();

	cmd->texture        = texture_stack.empty() ? nullptr : texture_stack.back();
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