#pragma once

#include "texture.h"
#include "glyph.h"

#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <cstddef>
#include <cstdint>

VKDL_BEGIN

struct FontInfo
{
	std::string family;
};

class Font
{
	struct FontHandles;

	using GlyphTable = std::unordered_map<std::uint64_t, Glyph>;

	struct Row
	{
		Row(uint32_t rowTop, uint32_t rowHeight) :
			width(0),
			top(rowTop),
			height(rowHeight)
		{
		}

		uint32_t width;
		uint32_t top;
		uint32_t height;
	};

	struct Page
	{
		explicit Page(bool smooth);

		GlyphTable       glyphs;
		Texture          texture;
		uint32_t         next_row;
		std::vector<Row> rows;
	};

	using PageTable = std::unordered_map<uint32_t, Page>;

public:
	Font();
	Font(const char* path);
	Font(const void* data, size_t size_in_bytes);

	VKDL_NODISCARD bool loadFromFile(const char* path);
	VKDL_NODISCARD bool loadFromMemory(const void* data, size_t size_in_bytes);

	VKDL_NODISCARD const Glyph& getGlyph(std::uint32_t code_point, uint32_t character_size, bool bold, float outline_thickness = 0) const;
	VKDL_NODISCARD bool hasGlyph(std::uint32_t code_point) const;

	VKDL_NODISCARD float getKerning(std::uint32_t first, std::uint32_t second, uint32_t character_size, bool bold = false) const;
	VKDL_NODISCARD float getLineSpacing(uint32_t character_size) const;
	VKDL_NODISCARD float getUnderlinePosition(uint32_t character_size) const;
	VKDL_NODISCARD float getUnderlineThickness(uint32_t character_size) const;

	VKDL_NODISCARD const Texture& getTexture(uint32_t character_size) const;

	// void setSmooth(bool smooth);
	// VKDL_NODISCARD bool isSmooth() const;

	VKDL_NODISCARD const FontInfo& getInfo() const;

private:
	void cleanup();

	Page& loadPage(uint32_t character_size) const;
	Glyph loadGlyph(std::uint32_t code_point, uint32_t character_size, bool bold, float outline_thickness) const;
	irect findGlyphRect(Page& page, uvec2 size) const;

	VKDL_NODISCARD bool setCurrentSize(uint32_t character_size) const;

	std::shared_ptr<FontHandles>      font_handles;
	bool                              is_smooth;
	FontInfo                          info;
	mutable PageTable                 pages;
	mutable std::vector<std::uint8_t> pixel_buffer;
};

VKDL_END