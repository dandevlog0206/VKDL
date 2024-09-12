#include "../include/vkdl/graphics/font.h"

#include "../include/vkdl/core/context.h"
#include "../include/vkdl/core/builtin_objects.h"

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_OUTLINE_H
#include FT_BITMAP_H
#include FT_STROKER_H

#define TEXTURE_MAXIMUM_SIZE 8192

template <typename T, typename U>
static T reinterpret(const U& input)
{
	T output;
	std::memcpy(&output, &input, sizeof(U));
	return output;
}

static uint64_t combine(float outline_thickness, bool bold, std::uint32_t index)
{
	return (uint64_t{ reinterpret<std::uint32_t>(outline_thickness) } << 32) | (std::uint64_t{ bold } << 31) | index;
}

VKDL_BEGIN

struct Font::FontHandles
{
	FontHandles() :
		library(nullptr),
		stream_rec(),
		face(nullptr),
		stroker(nullptr)
	
	{
	}

	~FontHandles()
	{
		// All the function below are safe to call with null pointer arguments.
		// The documentation of FreeType isn't clear on the matter, but the
		// implementation does explicitly check for null.

		FT_Stroker_Done(stroker);
		FT_Done_Face(face);
		// `streamRec` doesn't need to be explicitly freed.
		FT_Done_FreeType(library);
	}

	// clang-format off
	FontHandles(const FontHandles&) = delete;
	FontHandles& operator=(const FontHandles&) = delete;

	FontHandles(FontHandles&&) = delete;
	FontHandles& operator=(FontHandles&&) = delete;
	// clang-format on

	FT_Library   library;
	FT_StreamRec stream_rec;
	FT_Face      face;
	FT_Stroker   stroker;
};

Font::Page::Page(bool smooth) :
	next_row(3)
{
	ColorImage image(Colors::Transparent, 128, 128);

	image.at(0, 0) = Colors::White;
	image.at(0, 1) = Colors::White;
	image.at(1, 0) = Colors::White;
	image.at(1, 1) = Colors::White;

	auto& ctx             = Context::get();
	auto& desc_set_layout = ctx.getPipeline(VKDL_BUILTIN_PIPELINE0_UUID).getPipelineLayout().getDescriptorSetLayout(0);
	
	texture = TextureCreator()
		.setImageFormat(image.format())
		.setImageUsage(vk::ImageUsageFlagBits::eSampled 
			| vk::ImageUsageFlagBits::eTransferDst
			| vk::ImageUsageFlagBits::eTransferSrc)
		.setImageExtent(image.width(), image.height())
		.setDescriptorSetLayout(desc_set_layout)
		.create();

	texture.update(image.data());
}

Font::Font() :
	is_smooth(false)
{
}

Font::Font(const char* path)
{
	VKDL_CHECK_MSG(loadFromFile(path), "Failed to open font from file");
}

Font::Font(const void* data, size_t size_in_bytes)
{
	VKDL_CHECK_MSG(loadFromMemory(data, size_in_bytes), "Failed to open font from memory");
}

VKDL_NODISCARD bool Font::loadFromFile(const char* path)
{
	cleanup();

	auto handle = std::make_shared<FontHandles>();

	VKDL_CHECK_MSG(FT_Init_FreeType(&handle->library) == 0,
		"Failed to load font (failed to initialize FreeType)");

	VKDL_CHECK_MSG(FT_New_Face(handle->library, path, 0, &handle->face) == 0,
		"Failed to load font (failed to create the font face)");

	VKDL_CHECK_MSG(FT_Stroker_New(handle->library, &handle->stroker) == 0,
		"Failed to load font (failed to create the stroker)")

	VKDL_CHECK_MSG(FT_Select_Charmap(handle->face, FT_ENCODING_UNICODE) == 0,
		"Failed to load font (failed to set the Unicode character set)");

	info.family  = handle->face->family_name ? handle->face->family_name : std::string();
	font_handles = std::move(handle);

	return true;
}

VKDL_NODISCARD bool Font::loadFromMemory(const void* data, size_t size_in_bytes)
{
	cleanup();

	auto handle = std::make_shared<FontHandles>();

	VKDL_CHECK_MSG(FT_Init_FreeType(&handle->library) == 0,
		"Failed to load font (failed to initialize FreeType)");

	VKDL_CHECK_MSG(FT_New_Memory_Face(
		handle->library,
		reinterpret_cast<const FT_Byte*>(data),
		static_cast<FT_Long>(size_in_bytes),
		0,
		&handle->face) == 0,
		"Failed to load font (failed to create the font face)");

	VKDL_CHECK_MSG(FT_Stroker_New(handle->library, &handle->stroker) == 0,
		"Failed to load font (failed to create the stroker)")

	VKDL_CHECK_MSG(FT_Select_Charmap(handle->face, FT_ENCODING_UNICODE) == 0,
		"Failed to load font (failed to set the Unicode character set)");


	info.family  = handle->face->family_name ? handle->face->family_name : std::string();
	font_handles = std::move(handle);

	return true;
}

VKDL_NODISCARD const Glyph& Font::getGlyph(std::uint32_t code_point, uint32_t character_size, bool bold, float outline_thickness) const
{
	GlyphTable& glyphs = loadPage(character_size).glyphs;

	const std::uint64_t key = combine(outline_thickness,
		bold,
		FT_Get_Char_Index(font_handles ? font_handles->face : nullptr, code_point));

	if (const auto it = glyphs.find(key); it != glyphs.end())
		return it->second;

	const Glyph glyph = loadGlyph(code_point, character_size, bold, outline_thickness);
	
	return glyphs.emplace(key, glyph).first->second;
}

VKDL_NODISCARD bool Font::hasGlyph(std::uint32_t code_point) const
{
	return FT_Get_Char_Index(font_handles ? font_handles->face : nullptr, code_point) != 0;
}

VKDL_NODISCARD float Font::getKerning(std::uint32_t first, std::uint32_t second, uint32_t character_size, bool bold) const
{
	if (first == 0 || second == 0) return 0.f;

	FT_Face face = font_handles ? font_handles->face : nullptr;

	if (face && setCurrentSize(character_size)) {
		const FT_UInt index1 = FT_Get_Char_Index(face, first);
		const FT_UInt index2 = FT_Get_Char_Index(face, second);

		const auto firstRsbDelta = static_cast<float>(getGlyph(first, character_size, bold).rsb_delta);
		const auto secondLsbDelta = static_cast<float>(getGlyph(second, character_size, bold).lsb_delta);

		FT_Vector kerning{ 0, 0 };
		if (FT_HAS_KERNING(face))
			FT_Get_Kerning(face, index1, index2, FT_KERNING_UNFITTED, &kerning);

		if (!FT_IS_SCALABLE(face))
			return static_cast<float>(kerning.x);

		return std::floor((secondLsbDelta - firstRsbDelta + static_cast<float>(kerning.x) + 32) / float{ 1 << 6 });
	}

	return 0.f;
}

VKDL_NODISCARD float Font::getLineSpacing(uint32_t character_size) const
{
	FT_Face face = font_handles ? font_handles->face : nullptr;

	if (face && setCurrentSize(character_size))
		return static_cast<float>(face->size->metrics.height) / float{ 1 << 6 };

	return 0.f;
}

VKDL_NODISCARD float Font::getUnderlinePosition(uint32_t character_size) const
{
	FT_Face face = font_handles ? font_handles->face : nullptr;

	if (face && setCurrentSize(character_size)) {
		if (!FT_IS_SCALABLE(face))
			return static_cast<float>(character_size) / 10.f;
		return -static_cast<float>(FT_MulFix(face->underline_position, face->size->metrics.y_scale)) / float{ 1 << 6 };
	}

	return 0.f;
}

VKDL_NODISCARD float Font::getUnderlineThickness(uint32_t character_size) const
{
	FT_Face face = font_handles ? font_handles->face : nullptr;

	if (face && setCurrentSize(character_size))	{
		if (!FT_IS_SCALABLE(face))
			return static_cast<float>(character_size) / 14.f;
		return static_cast<float>(FT_MulFix(face->underline_thickness, face->size->metrics.y_scale)) / float{ 1 << 6 };
	}

	return 0.f;
}

VKDL_NODISCARD const Texture& Font::getTexture(uint32_t character_size) const
{
	return loadPage(character_size).texture;
}

//void Font::setSmooth(bool smooth)
//{
//	if (smooth != is_smooth)
//	{
//		is_smooth = smooth;
//
//		for (auto& [key, page] : pages)
//		{
//			page.texture.setSmooth(is_smooth);
//		}
//	}
//}
//
//VKDL_NODISCARD bool Font::isSmooth() const
//{
//	return is_smooth;
//}

VKDL_NODISCARD const FontInfo& Font::getInfo() const
{
	return info;
}

void Font::cleanup()
{
	font_handles.reset();
	pages.clear();
	pixel_buffer.clear();
}

Font::Page& Font::loadPage(uint32_t character_size) const
{
	return pages.try_emplace(character_size, is_smooth).first->second;
}

Glyph Font::loadGlyph(std::uint32_t code_point, uint32_t character_size, bool bold, float outline_thickness) const
{
	Glyph glyph;

	if (!font_handles)
		return glyph;

	FT_Face face = font_handles->face;
	if (!face)
		return glyph;

	if (!setCurrentSize(character_size))
		return glyph;

	FT_Int32 flags = FT_LOAD_TARGET_NORMAL | FT_LOAD_FORCE_AUTOHINT;
	if (outline_thickness != 0)
		flags |= FT_LOAD_NO_BITMAP;
	if (FT_Load_Char(face, code_point, flags) != 0)
		return glyph;

	FT_Glyph glyphDesc = nullptr;
	if (FT_Get_Glyph(face->glyph, &glyphDesc) != 0)
		return glyph;

	const FT_Pos weight  = 1 << 6;
	const bool   outline = (glyphDesc->format == FT_GLYPH_FORMAT_OUTLINE);
	
	if (outline) {
		if (bold) {
			auto* outlineGlyph = reinterpret_cast<FT_OutlineGlyph>(glyphDesc);
			FT_Outline_Embolden(&outlineGlyph->outline, weight);
		}

		if (outline_thickness != 0) {
			FT_Stroker stroker = font_handles->stroker;

			FT_Stroker_Set(stroker,
				static_cast<FT_Fixed>(outline_thickness * float{ 1 << 6 }),
				FT_STROKER_LINECAP_ROUND,
				FT_STROKER_LINEJOIN_ROUND,
				0);
			FT_Glyph_Stroke(&glyphDesc, stroker, true);
		}
	}

	FT_Glyph_To_Bitmap(&glyphDesc, FT_RENDER_MODE_NORMAL, nullptr, 1);
	auto* bitmapGlyph = reinterpret_cast<FT_BitmapGlyph>(glyphDesc);
	FT_Bitmap& bitmap = bitmapGlyph->bitmap;

	if (!outline)
	{
		if (bold)
			FT_Bitmap_Embolden(font_handles->library, &bitmap, weight, weight);

		VKDL_CHECK_MSG(outline_thickness == 0,
			"Failed to outline glyph (no fallback available)");
	}

	glyph.advance = static_cast<float>(bitmapGlyph->root.advance.x >> 16);
	if (bold)
		glyph.advance += static_cast<float>(weight) / float{ 1 << 6 };

	glyph.lsb_delta = static_cast<int>(face->glyph->lsb_delta);
	glyph.rsb_delta = static_cast<int>(face->glyph->rsb_delta);

	uvec2 size(bitmap.width, bitmap.rows);

	if ((size.x > 0) && (size.y > 0)) {
		const uint32_t padding = 2;

		size += 2u * uvec2(padding, padding);

		Page& page = loadPage(character_size);

		glyph.texture_rect = findGlyphRect(page, size);

		glyph.texture_rect.position += ivec2(padding, padding);
		glyph.texture_rect.size     -= 2 * ivec2(padding, padding);

		glyph.bounds.position = vec2(ivec2(bitmapGlyph->left, -bitmapGlyph->top));
		glyph.bounds.size     = vec2(uvec2(bitmap.width, bitmap.rows));

		pixel_buffer.resize(static_cast<std::size_t>(size.x) * static_cast<std::size_t>(size.y) * 4);

		std::uint8_t* current = pixel_buffer.data();
		std::uint8_t* end = current + size.x * size.y * 4;

		while (current != end) {
			(*current++) = 255;
			(*current++) = 255;
			(*current++) = 255;
			(*current++) = 0;
		}

		const std::uint8_t* pixels = bitmap.buffer;
		if (bitmap.pixel_mode == FT_PIXEL_MODE_MONO) {
			for (uint32_t y = padding; y < size.y - padding; ++y) {
				for (uint32_t x = padding; x < size.x - padding; ++x) {
					const std::size_t index = x + y * size.x;
					pixel_buffer[index * 4 + 3] = ((pixels[(x - padding) / 8]) & (1 << (7 - ((x - padding) % 8)))) ? 255 : 0;
				}
				pixels += bitmap.pitch;
			}
		} else {
			for (uint32_t y = padding; y < size.y - padding; ++y) {
				for (uint32_t x = padding; x < size.x - padding; ++x) {
					const std::size_t index = x + y * size.x;
					pixel_buffer[index * 4 + 3] = pixels[x - padding];
				}
				pixels += bitmap.pitch;
			}
		}

		const auto dest = uvec2(glyph.texture_rect.position) - uvec2(padding, padding);
		const auto updateSize = uvec2(glyph.texture_rect.size) + 2u * uvec2(padding, padding);
		page.texture.update(pixel_buffer.data(), (ivec2)dest, updateSize);
	}

	FT_Done_Glyph(glyphDesc);

	return glyph;
}

irect Font::findGlyphRect(Page& page, uvec2 size) const
{
	Row* row = nullptr;
	float bestRatio = 0;
	for (auto it = page.rows.begin(); it != page.rows.end() && !row; ++it)
	{
		const float ratio = static_cast<float>(size.y) / static_cast<float>(it->height);

		// Ignore rows that are either too small or too high
		if ((ratio < 0.7f) || (ratio > 1.f))
			continue;

		// Check if there's enough horizontal space left in the row
		if (size.x > page.texture.extent().x - it->width)
			continue;

		// Make sure that this new row is the best found so far
		if (ratio < bestRatio)
			continue;

		// The current row passed all the tests: we can select it
		row = &*it;
		bestRatio = ratio;
	}

	// If we didn't find a matching row, create a new one (10% taller than the glyph)
	if (!row) {
		const uint32_t rowHeight = size.y + size.y / 10;
		while ((page.next_row + rowHeight >= page.texture.extent().y) || (size.x >= page.texture.extent().x)) {
			const uvec2 textureSize = page.texture.extent();
			if ((textureSize.x * 2 <= TEXTURE_MAXIMUM_SIZE) && (textureSize.y * 2 <= TEXTURE_MAXIMUM_SIZE)) {
				page.texture.resize(2u * textureSize.x, 2u * textureSize.y);
			} else {
				VKDL_ERROR("Failed to add a new character to the font: the maximum texture size has been reached");
				return { {0, 0}, {2, 2} };
			}
		}

		page.rows.emplace_back(page.next_row, rowHeight);
		page.next_row += rowHeight;
		row = &page.rows.back();
	}

	irect rect({ row->width, row->top }, size);
	row->width += size.x;

	return rect;
}

VKDL_NODISCARD bool Font::setCurrentSize(uint32_t character_size) const
{
	FT_Face         face = font_handles->face;
	const FT_UShort currentSize = face->size->metrics.x_ppem;

	if (currentSize != character_size) {
		const FT_Error result = FT_Set_Pixel_Sizes(face, 0, character_size);
		
		VKDL_CHECK_MSG(result != FT_Err_Invalid_Pixel_Size,
			"Failed to set bitmap font size to " + std::to_string(character_size));

		return result == FT_Err_Ok;
	}

	return true;
}

VKDL_END