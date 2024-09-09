#pragma once

#include "image.h"
#include "color.h"
#include "../core/descriptor_set.h"
#include "../core/buffer.h"

VKDL_BEGIN

class Texture;

class TextureCreator
{
public:
	TextureCreator();

	TextureCreator& setImageFormat(vk::Format format);
	TextureCreator& setImageExtent(uint32_t width, uint32_t height);
	TextureCreator& setImageExtent(vk::Extent2D extent);
	TextureCreator& setImageExtent(uint32_t width, uint32_t height, uint32_t depth);
	TextureCreator& setImageExtent(vk::Extent3D extent);
	TextureCreator& setImageMipLevels(uint32_t mip_levels);
	TextureCreator& setImageSamples(vk::SampleCountFlagBits samples);
	TextureCreator& setImageTiling(vk::ImageTiling tiling);
	TextureCreator& setImageUsage(vk::ImageUsageFlags usage);
	TextureCreator& setImageSharingMode(vk::SharingMode mode);
	TextureCreator& setImageMemoryProperties(vk::MemoryPropertyFlags props);

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

	std::shared_ptr<Texture> create() const;

private:
	vk::DeviceMemory allocateMemory(vk::Image image, vk::DeviceSize& size) const;
	vk::ImageView createImageView(vk::Image image) const;
	vk::Sampler createSampler() const;

private:
	vk::ImageCreateInfo     image_info;
	vk::SamplerCreateInfo   sampler_info;
	vk::MemoryPropertyFlags memory_props;
	vk::DescriptorSetLayout desc_set_layout;
};

class Texture : public std::enable_shared_from_this<Texture>
{
	friend class TextureCreator;

	VKDL_NOCOPY(Texture);
	VKDL_NOMOVE(Texture);
	VKDL_NOCOPYASS(Texture);
	VKDL_NOMOVEASS(Texture);
	
	Texture();

public:
	~Texture();

	void update(void* pixels);

	vk::DescriptorSet getDescriptorSet() const;

private:
	vk::DeviceMemory  memory;
	vk::Image         image;
	vk::ImageView     image_view;
	vk::Sampler       sampler;
	vk::DescriptorSet desc_set;
	Buffer<uint8_t>   staging_buffer;

	vk::Extent3D   extent;
	vk::DeviceSize allocated_size;
	vk::Format     format;
};

VKDL_END