#include "../core/buffer.h"
#include "../core/drawable.h"
#include "../math/transform_2d.h"
#include "vertex.h"

VKDL_BEGIN

class Texture;
class Font;
class Glyph;

struct TextStyle
{
	const Font*  font                  = nullptr;
	unsigned int character_size        = 30;
	float        align_h               = 0.f;
	float        align_v               = 1.f;
	float        letter_spacing_factor = 1.f;
	float        line_spacing_factor   = 1.f;
	Color        fill_color            = Colors::White;

	bool bold           = false;
	bool italic         = false;
	bool underline      = false;
	bool strike_through = false;
};

struct DrawCommand2D
{
public:
	DrawCommand2D();

	const Texture* texture;
	vk::Rect2D     clip_rect;
	Transform2D    transform;

	uint32_t vertex_offset;
	uint32_t vertex_count;
	uint32_t index_offset;
	uint32_t index_count;
};

class DrawList2D : public Drawable
{
public:
	DrawList2D();
	
	void addRawTriangle(const Vertex2D& v0, const Vertex2D& v1, const Vertex2D& v2);

	void addDot(const vec2& p, float r, const Color& col);
	void addLine(const vec2& p0, const vec2& p1, float width, const Color& col);
	void addFilledTriangle(const vec2& p0, const vec2& p1, const vec2& p2, const Color& col);
	
	void addImage(const Texture& texture, const vec2& pos, const vec2& size, const vec2& uv0, const vec2& uv1, const Color& col = Colors::White);
	void addImage(const vec2& pos, const vec2& size, const vec2& uv0, const vec2& uv1, const Color& col = Colors::White);
	void addImageQuad(const vec2& p0, const vec2& p1, const vec2& p2, const vec2& p3, const vec2& uv0, const vec2& uv1, const vec2& uv2, const vec2& uv3, const Color& col = Colors::White);

	void addText(const vec2& pos, const std::string& text, const TextStyle& style);
	void addGlyphQuad(const vec2& pos, const Color& color, const Glyph& glyph, float italicShear);
	void addTextLine(float length, float top, const Color& color, float offset, float thickness, float outlineThickness = 0);

	void pushTexture(const Texture& texture);
	void popTexture();

	void pushClipRectFullScreen();
	void pushClipRect(vk::Rect2D rect, bool intersect = false);
	void pushClipRect(int32_t off_x, int32_t off_y, uint32_t width, uint32_t height, bool intersect = false);
	void popClipRect();

	void pushTransform(const Transform2D& transform);
	void popTransform();

	void clear();

private:
	void draw(RenderTarget& target, RenderStates& states, const RenderOptions& options) const override;

	void newCommand();
	uint32_t reservePrimitives(uint32_t vert_size, uint32_t idx_size);

private:
	std::vector<DrawCommand2D> commands;
	std::vector<Vertex2D>      vertices;
	std::vector<uint32_t>      indices;
	
	std::vector<const Texture*> texture_stack;
	std::vector<vk::Rect2D>     clip_rect_stack;
	std::vector<Transform2D>    transform_stack;
	
	mutable Buffer<Vertex2D>   vertex_buffer;
	mutable Buffer<uint32_t>   index_buffer;
	mutable bool               update_buffer;
};

VKDL_END