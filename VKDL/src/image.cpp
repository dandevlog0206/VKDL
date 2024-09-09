#include "../include/vkdl/graphics/image.h"

#define __STDC_LIB_EXT1__
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

static void* new_malloc(size_t size)
{
	return static_cast<void*>(new uint8_t[size]);
}

static void* new_realloc(void* block, size_t size)
{
	delete[] static_cast<uint8_t*>(block);
	return static_cast<void*>(new uint8_t[size]);
}

static void new_free(void* block)
{
	delete[] static_cast<uint8_t*>(block);
}

#define STBI_MALLOC(sz)       new_malloc(sz)
#define STBI_REALLOC(p,newsz) new_realloc(p,newsz)
#define STBI_FREE(p)          new_free(p)

#include "../../thirdparty/stb/stb_image.h"
#include "../../thirdparty/stb/stb_image_write.h"

VKDL_BEGIN

ColorImage::ColorImage() :
	Image()
{
}

ColorImage::ColorImage(uint32_t width, uint32_t height) :
	Image(width, height, vk::Format::eR8G8B8A8Unorm)
{
}

ColorImage::ColorImage(Color value, uint32_t width, uint32_t height) :
	Image(value, width, height, vk::Format::eR8G8B8A8Unorm)
{
}

ColorImage::ColorImage(Color* pixels, uint32_t width, uint32_t height) :
	Image(pixels, width, height, vk::Format::eR8G8B8A8Unorm)
{
}

ColorImage::ColorImage(const char* path)
{
	loadFromFile(path);
}

bool ColorImage::loadFromFile(const char* path)
{
	int w, h, comp = 0;
	pixels = (Color*)stbi_load(path, &w, &h, &comp, STBI_rgb_alpha);

	if (pixels == nullptr) return false;

	img_format = vk::Format::eR8G8B8A8Unorm;
	img_w      = (uint32_t)w;
	img_h      = (uint32_t)h;

	return true;
}

bool ColorImage::saveToFile(const char* path)
{
	return stbi_write_png(path, (int)img_w, (int)img_h, 4, pixels, 0);
}

ColorImage ColorImage::blit(const uvec2& offset, const uvec2& size) const
{
	VKDL_CHECK(offset.x + size.x < img_w);
	VKDL_CHECK(offset.x + size.x < img_h);

	ColorImage new_image(size.x, size.y);

	size_t stride = size.x * sizeof(Color);

	for (uint32_t h = 0; h < size.y; ++h) {
		const Color* begin = &at(offset.x, offset.y + h);
		memcpy(&new_image.at(0, h), begin, stride);
	}

	return new_image;
}

ColorImage ColorImage::createMask(Color from, Color to) const
{
	ColorImage image(pixels, img_w, img_h);

	for (size_t i = 0; i < img_w * img_h; ++i)
		if (image.data()[i] == from)
			image.data()[i] = to;

	return image;
}

VKDL_END