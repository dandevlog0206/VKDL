#pragma once

#include "../core/include_vulkan.h"
#include "color.h"

VKDL_BEGIN

template <class DataType>
class Image
{
public:
	using pixel_type = DataType;

	Image();
	Image(const Image& rhs);
	Image(Image&& rhs);
	Image(uint32_t width, uint32_t height, vk::Format format);
	Image(DataType value, uint32_t width, uint32_t height, vk::Format format);
	Image(DataType* pixels, uint32_t width, uint32_t height, vk::Format format);
	~Image();

	Image& operator=(const Image& rhs);
	Image& operator=(Image&& rhs);

	void create(DataType value, uint32_t width, uint32_t height, vk::Format format);

	void loadFromMemory(DataType* pixels, uint32_t width, uint32_t height, vk::Format format);

	void clear();
	void* release();

	vk::Format format() const;
	uint32_t width() const;
	uint32_t height() const;

	const DataType& at(uint32_t x, uint32_t y) const;
	DataType& at(uint32_t x, uint32_t y);

	const DataType* data() const;
	DataType* data();

	bool empty() const;

protected:
	DataType*  pixels;
	vk::Format img_format;
	uint32_t   img_w;
	uint32_t   img_h;
};

class ColorImage : public Image<Color>
{
public:
	using pixel_type = Color;

	ColorImage();
	ColorImage(uint32_t width, uint32_t height);
	ColorImage(Color value, uint32_t width, uint32_t height);
	ColorImage(Color* pixels, uint32_t width, uint32_t height);
	ColorImage(const char* path);

	bool loadFromFile(const char* path);
	bool saveToFile(const char* path);

	ColorImage blit(const uvec2& offset, const uvec2& size) const;
	ColorImage createMask(Color color_from, Color color_to = Colors::Transparent) const;
};

template<class DataType>
Image<DataType>::Image() :
	pixels(nullptr),
	img_format(),
	img_w(0),
	img_h(0)
{
}

template<class DataType>
Image<DataType>::Image(const Image& rhs) :
	pixels(new DataType[rhs.img_w * rhs.img_h]),
	img_format(rhs.img_format),
	img_w(rhs.img_w),
	img_h(rhs.img_h)
{
	memcpy(pixels, rhs.pixels, sizeof(DataType) * img_w * img_h);
}

template<class DataType>
Image<DataType>::Image(Image&& rhs) :
	pixels(std::exchange(rhs.pixels, nullptr)),
	img_format(std::exchange(rhs.img_format, {})),
	img_w(std::exchange(rhs.img_w, 0)),
	img_h(std::exchange(rhs.img_h, 0))
{
}

template<class DataType>
Image<DataType>::Image(uint32_t width, uint32_t height, vk::Format format) :
	pixels(new DataType[width * height]),
	img_format(format),
	img_w(width),
	img_h(height)
{
}

template<class DataType>
Image<DataType>::Image(DataType value, uint32_t width, uint32_t height, vk::Format format) :
	pixels(new DataType[width * height]),
	img_format(format),
	img_w(width),
	img_h(height)
{
	std::fill_n(pixels, width * height, value);
}

template<class DataType>
Image<DataType>::Image(DataType* pixels, uint32_t width, uint32_t height, vk::Format format) :
	pixels(pixels),
	img_format(format),
	img_w(width),
	img_h(height)
{
}

template<class DataType>
Image<DataType>::~Image()
{
	delete[] pixels;
}

template<class DataType>
Image<DataType>& Image<DataType>::operator=(const Image<DataType>& rhs)
{
	if (img_w * img_h != rhs.img_w * rhs.img_h) {
		delete[] pixels;
		pixels = new DataType[rhs.img_w * rhs.img_h];
	}

	img_format = rhs.img_format;
	img_w      = rhs.img_w;
	img_h      = rhs.img_h;

	memcpy(pixels, rhs.pixels, sizeof(DataType) * img_w * img_h);
}

template<class DataType>
Image<DataType>& Image<DataType>::operator=(Image<DataType>&& rhs)
{
	pixels     = std::exchange(rhs.pixels, nullptr);
	img_format = std::exchange(rhs.img_format, {});
	img_w      = std::exchange(rhs.img_w, 0);
	img_h      = std::exchange(rhs.img_h, 0);
}

template<class DataType>
void Image<DataType>::create(DataType value, uint32_t width, uint32_t height, vk::Format format)
{
	pixels = new DataType[width * height];
	std::fill_n(pixels, width * height, value);
}

template<class DataType>
void Image<DataType>::loadFromMemory(DataType* pixels, uint32_t width, uint32_t height, vk::Format format)
{
	this->pixels = pixels;
	img_format   = format;
	img_w        = width;
	img_h        = height;
}

template<class DataType>
void Image<DataType>::clear()
{
	img_format = {};
	img_w      = 0;
	img_h      = 0;
	delete[] std::exchange(pixels, nullptr);
}

template<class DataType>
void* Image<DataType>::release()
{
	img_format = {};
	img_w      = 0;
	img_h      = 0;
	return std::exchange(pixels, nullptr);
}

template<class DataType>
vk::Format Image<DataType>::format() const
{
	return img_format;
}

template<class DataType>
uint32_t Image<DataType>::width() const
{
	return img_w;
}

template<class DataType>
uint32_t Image<DataType>::height() const
{
	return img_h;
}

template<class DataType>
const DataType& Image<DataType>::at(uint32_t x, uint32_t y) const
{
	return *(pixels + img_w * y + x);
}

template<class DataType>
DataType& Image<DataType>::at(uint32_t x, uint32_t y)
{
	return *(pixels + img_w * y + x);
}

template<class DataType>
const DataType* Image<DataType>::data() const
{
	return pixels;
}

template<class DataType>
DataType* Image<DataType>::data()
{
	return pixels;
}

template<class DataType>
bool Image<DataType>::empty() const
{
	return pixels == nullptr;
}

VKDL_END