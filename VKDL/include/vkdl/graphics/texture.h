#pragma once

#include "image.h"
#include "color.h"
#include "../core/descriptor_set.h"
#include "../core/buffer.h"

VKDL_BEGIN

struct TextureInfo
{
	vk::ImageCreateInfo     image_info;
	vk::SamplerCreateInfo   sampler_info;
	vk::DescriptorSetLayout desc_set_layout;
	vk::MemoryPropertyFlags memory_props;
};

class Texture : public std::enable_shared_from_this<Texture>
{
	friend class TextureCreator;

	VKDL_NOCOPY(Texture);
	VKDL_NOCOPYASS(Texture);
	
	Texture(const TextureInfo& info);

public:
	Texture();
	Texture(Texture&& rhs);
	~Texture();

	Texture& operator=(Texture&& rhs);

	const vk::DescriptorSet& getDescriptorSet() const;

	void update(void* pixels);
	void update(void* pixels, const ivec2& offset, const uvec2& size);

	void resize(uint32_t width, uint32_t height);

	void clear();

	TextureInfo getTextureInfo() const;

	vk::Format format() const;

	uvec2 extent() const;
	size_t size_in_bytes() const;
	size_t capacity() const;

	bool is_null() const;
	bool empty() const;

	void swap(Texture& rhs);

private:
	vk::DeviceMemory allocateMemory(vk::Image image, vk::DeviceSize& size) const;
	vk::DescriptorSet createDescriptorSet(vk::Sampler sampler, vk::ImageView image_view) const;

private:
	TextureInfo       info;

	vk::Image         image;
	vk::ImageView     image_view;
	vk::DeviceMemory  memory;
	vk::Sampler       sampler;
	vk::DescriptorSet desc_set;
	vk::DeviceSize    allocated_size;
	Buffer<uint8_t>   staging_buffer;
};

class TextureCreator
{
public:
	TextureCreator();

	TextureCreator& setImageFormat(vk::Format format);
	TextureCreator& setImageExtent(uint32_t width, uint32_t height);
	TextureCreator& setImageExtent(vk::Extent2D extent);
	TextureCreator& setImageMipLevels(uint32_t mip_levels);
	TextureCreator& setImageSamples(vk::SampleCountFlagBits samples);
	TextureCreator& setImageTiling(vk::ImageTiling tiling);
	TextureCreator& setImageUsage(vk::ImageUsageFlags usage);
	TextureCreator& setImageSharingMode(vk::SharingMode mode);

	TextureCreator& setMemoryProperties(vk::MemoryPropertyFlags props);

	TextureCreator& setSamplerMagFilter(vk::Filter filter);
	TextureCreator& setSamplerMinFilter(vk::Filter filter);
	TextureCreator& setSamplerMipMapMode(vk::SamplerMipmapMode mode);
	TextureCreator& setSamplerAddressMode(vk::SamplerAddressMode U, vk::SamplerAddressMode V, vk::SamplerAddressMode W);
	TextureCreator& setSamplerMipLodBias(bool value);
	TextureCreator& setSamplerAnisotropy(float max_anisotropy);
	TextureCreator& setSamplerCompare(vk::CompareOp op);
	TextureCreator& setSamplerLod(float min, float max);
	TextureCreator& setSamplerBorderColor(vk::BorderColor color);
	TextureCreator& setSamplerUnnormalizedCoordinates(bool value);

	TextureCreator& setDescriptorSetLayout(vk::DescriptorSetLayout layout);

	Texture create() const;

private:
	TextureInfo info;
};

VKDL_END