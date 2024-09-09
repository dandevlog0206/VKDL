#include "../include/vkdl/graphics/texture.h"

#include "../include/vkdl/core/context.h"

vk::ImageViewType to_image_view_type(vk::ImageType type) {
	switch (type) {
	case vk::ImageType::e1D: return vk::ImageViewType::e1D;
	case vk::ImageType::e2D: return vk::ImageViewType::e2D;
	case vk::ImageType::e3D: return vk::ImageViewType::e3D;
	default: VKDL_ERROR("Unknown image type");
	};
}

size_t format_size_in_byte(vk::Format format)
{
	switch (format)
	{
	case vk::Format::eR4G4UnormPack8:
	case vk::Format::eR8Unorm:
	case vk::Format::eR8Snorm:
	case vk::Format::eR8Uscaled:
	case vk::Format::eR8Sscaled:
	case vk::Format::eR8Uint:
	case vk::Format::eR8Sint:
	case vk::Format::eR8Srgb:
		return 1;
	case vk::Format::eR4G4B4A4UnormPack16:
	case vk::Format::eB4G4R4A4UnormPack16:
	case vk::Format::eR5G6B5UnormPack16:
	case vk::Format::eB5G6R5UnormPack16:
	case vk::Format::eR5G5B5A1UnormPack16:
	case vk::Format::eB5G5R5A1UnormPack16:
	case vk::Format::eA1R5G5B5UnormPack16:
	case vk::Format::eR8G8Unorm:
	case vk::Format::eR8G8Snorm:
	case vk::Format::eR8G8Uscaled:
	case vk::Format::eR8G8Sscaled:
	case vk::Format::eR8G8Uint:
	case vk::Format::eR8G8Sint:
	case vk::Format::eR8G8Srgb:
	case vk::Format::eR16Unorm:
	case vk::Format::eR16Snorm:
	case vk::Format::eR16Uscaled:
	case vk::Format::eR16Sscaled:
	case vk::Format::eR16Uint:
	case vk::Format::eR16Sint:
	case vk::Format::eR16Sfloat:
		return 2;
	case vk::Format::eR8G8B8Unorm:
	case vk::Format::eR8G8B8Snorm:
	case vk::Format::eR8G8B8Uscaled:
	case vk::Format::eR8G8B8Sscaled:
	case vk::Format::eR8G8B8Uint:
	case vk::Format::eR8G8B8Sint:
	case vk::Format::eR8G8B8Srgb:
	case vk::Format::eB8G8R8Unorm:
	case vk::Format::eB8G8R8Snorm:
	case vk::Format::eB8G8R8Uscaled:
	case vk::Format::eB8G8R8Sscaled:
	case vk::Format::eB8G8R8Uint:
	case vk::Format::eB8G8R8Sint:
	case vk::Format::eB8G8R8Srgb:
		return 3;
	case vk::Format::eR32Uint:
	case vk::Format::eR32Sint:
	case vk::Format::eR32Sfloat:
	case vk::Format::eR16G16Unorm:
	case vk::Format::eR16G16Snorm:
	case vk::Format::eR16G16Uscaled:
	case vk::Format::eR16G16Sscaled:
	case vk::Format::eR16G16Uint:
	case vk::Format::eR16G16Sint:
	case vk::Format::eR16G16Sfloat:
	case vk::Format::eR8G8B8A8Unorm:
	case vk::Format::eR8G8B8A8Snorm:
	case vk::Format::eR8G8B8A8Uscaled:
	case vk::Format::eR8G8B8A8Sscaled:
	case vk::Format::eR8G8B8A8Uint:
	case vk::Format::eR8G8B8A8Sint:
	case vk::Format::eR8G8B8A8Srgb:
	case vk::Format::eB8G8R8A8Unorm:
	case vk::Format::eB8G8R8A8Snorm:
	case vk::Format::eB8G8R8A8Uscaled:
	case vk::Format::eB8G8R8A8Sscaled:
	case vk::Format::eB8G8R8A8Uint:
	case vk::Format::eB8G8R8A8Sint:
	case vk::Format::eB8G8R8A8Srgb:
	case vk::Format::eA8B8G8R8UnormPack32:
	case vk::Format::eA8B8G8R8SnormPack32:
	case vk::Format::eA8B8G8R8UscaledPack32:
	case vk::Format::eA8B8G8R8SscaledPack32:
	case vk::Format::eA8B8G8R8UintPack32:
	case vk::Format::eA8B8G8R8SintPack32:
	case vk::Format::eA8B8G8R8SrgbPack32:
	case vk::Format::eA2R10G10B10UnormPack32:
	case vk::Format::eA2R10G10B10SnormPack32:
	case vk::Format::eA2R10G10B10UscaledPack32:
	case vk::Format::eA2R10G10B10SscaledPack32: 
	case vk::Format::eA2R10G10B10UintPack32:
	case vk::Format::eA2R10G10B10SintPack32:
	case vk::Format::eA2B10G10R10UnormPack32:
	case vk::Format::eA2B10G10R10SnormPack32:
	case vk::Format::eA2B10G10R10UscaledPack32:
	case vk::Format::eA2B10G10R10SscaledPack32:
	case vk::Format::eA2B10G10R10UintPack32:
	case vk::Format::eA2B10G10R10SintPack32:
	case vk::Format::eB10G11R11UfloatPack32:
	case vk::Format::eE5B9G9R9UfloatPack32:
		return 4;
	case vk::Format::eR16G16B16Unorm:
	case vk::Format::eR16G16B16Snorm:
	case vk::Format::eR16G16B16Uscaled:
	case vk::Format::eR16G16B16Sscaled:
	case vk::Format::eR16G16B16Uint:
	case vk::Format::eR16G16B16Sint:
	case vk::Format::eR16G16B16Sfloat:
		return 6;
	case vk::Format::eR64Uint:
	case vk::Format::eR64Sint:
	case vk::Format::eR64Sfloat:
	case vk::Format::eR32G32Uint:
	case vk::Format::eR32G32Sint:
	case vk::Format::eR32G32Sfloat:
	case vk::Format::eR16G16B16A16Unorm:
	case vk::Format::eR16G16B16A16Snorm:
	case vk::Format::eR16G16B16A16Uscaled:
	case vk::Format::eR16G16B16A16Sscaled:
	case vk::Format::eR16G16B16A16Uint:
	case vk::Format::eR16G16B16A16Sint:
	case vk::Format::eR16G16B16A16Sfloat:
		return 8;
	case vk::Format::eR32G32B32Uint:
	case vk::Format::eR32G32B32Sint:
	case vk::Format::eR32G32B32Sfloat:
		return 12;
	case vk::Format::eR64G64Uint:
	case vk::Format::eR64G64Sint:
	case vk::Format::eR64G64Sfloat:
	case vk::Format::eR32G32B32A32Uint:
	case vk::Format::eR32G32B32A32Sint:
	case vk::Format::eR32G32B32A32Sfloat:
		return 16;
	case vk::Format::eR64G64B64Uint:
	case vk::Format::eR64G64B64Sint:
	case vk::Format::eR64G64B64Sfloat:
		return 24;
	case vk::Format::eR64G64B64A64Uint:
	case vk::Format::eR64G64B64A64Sint:
	case vk::Format::eR64G64B64A64Sfloat:
		return 32;
	default: VKDL_ERROR("Unknown format");
	}
}

VKDL_BEGIN

TextureCreator::TextureCreator() :
	image_info(
		{},
		{},
		{},
		{},
		1,
		1,
		vk::SampleCountFlagBits::e1,
		{},
		{},
		vk::SharingMode::eExclusive,
		0,
		nullptr,
		vk::ImageLayout::eUndefined
	),
	sampler_info(
		{},
		vk::Filter::eLinear,
		vk::Filter::eLinear,
		vk::SamplerMipmapMode::eLinear,
		vk::SamplerAddressMode::eRepeat,
		vk::SamplerAddressMode::eRepeat,
		vk::SamplerAddressMode::eRepeat,
		false,
		false,
		1.f,
		false,
		vk::CompareOp::eNever,
		-1000.f,
		1000.f,
		vk::BorderColor::eFloatTransparentBlack,
		false
	)
{
}

TextureCreator& TextureCreator::setImageFormat(vk::Format format)
{
	image_info.format = format;
	return *this;
}

TextureCreator& TextureCreator::setImageExtent(uint32_t width, uint32_t height)
{
	image_info.extent    = vk::Extent3D{ width, height, 1 };
	image_info.imageType = vk::ImageType::e2D;
	return *this;
}

TextureCreator& TextureCreator::setImageExtent(vk::Extent2D extent)
{
	image_info.extent    = vk::Extent3D{ extent, 1 };
	image_info.imageType = vk::ImageType::e2D;
	return *this;
}

TextureCreator& TextureCreator::setImageExtent(uint32_t width, uint32_t height, uint32_t depth)
{
	image_info.extent    = vk::Extent3D{ width, height, depth };
	image_info.imageType = vk::ImageType::e3D;
	return *this;
}

TextureCreator& TextureCreator::setImageExtent(vk::Extent3D extent)
{
	image_info.extent    = extent;
	image_info.imageType = vk::ImageType::e3D;
	return *this;
}

TextureCreator& TextureCreator::setImageMipLevels(uint32_t mip_levels)
{
	image_info.mipLevels = mip_levels;
	return *this;
}

TextureCreator& TextureCreator::setImageSamples(vk::SampleCountFlagBits samples)
{
	image_info.samples = samples;
	return *this;
}

TextureCreator& TextureCreator::setImageTiling(vk::ImageTiling tiling)
{
	image_info.tiling = tiling;
	return *this;
}

TextureCreator& TextureCreator::setImageUsage(vk::ImageUsageFlags usage)
{
	image_info.usage = usage;
	return *this;
}

TextureCreator& TextureCreator::setImageSharingMode(vk::SharingMode mode)
{
	image_info.sharingMode = mode;
	return *this;
}

TextureCreator& TextureCreator::setImageMemoryProperties(vk::MemoryPropertyFlags props)
{
	memory_props = props;
	return *this;
}

TextureCreator& TextureCreator::setSamplerMagFilter(vk::Filter filter)
{
	sampler_info.magFilter = filter;
	return *this;
}

TextureCreator& TextureCreator::setSamplerMinFilter(vk::Filter filter)
{
	sampler_info.minFilter = filter;
	return *this;
}

TextureCreator& TextureCreator::setSamplerMipMapMode(vk::SamplerMipmapMode mode)
{
	sampler_info.mipmapMode = mode;
	return *this;
}

TextureCreator& TextureCreator::setSamplerAddressMode(vk::SamplerAddressMode U, vk::SamplerAddressMode V, vk::SamplerAddressMode W)
{
	sampler_info.addressModeU = U;
	sampler_info.addressModeV = V;
	sampler_info.addressModeW = W;
	return *this;
}

TextureCreator& TextureCreator::setSamplerMipLodBias(bool value)
{
	sampler_info.mipLodBias = value;
	return *this;
}

TextureCreator& TextureCreator::setSamplerAnisotropy(float max_anisotropy)
{
	sampler_info.anisotropyEnable = true;
	sampler_info.maxAnisotropy    = max_anisotropy;
	return *this;
}

TextureCreator& TextureCreator::setSamplerCompare(vk::CompareOp op)
{
	sampler_info.compareEnable = true;
	sampler_info.compareOp     = op;
	return *this;
}

TextureCreator& TextureCreator::setSamplerLod(float min, float max)
{
	sampler_info.minLod = min;
	sampler_info.maxLod = max;
	return *this;
}

TextureCreator& TextureCreator::setSamplerBorderColor(vk::BorderColor color)
{
	sampler_info.borderColor = color;
	return *this;
}

TextureCreator& TextureCreator::setSamplerUnnormalizedCoordinates(bool value)
{
	sampler_info.unnormalizedCoordinates = value;
	return *this;
}

TextureCreator& TextureCreator::setDescriptorSetLayout(vk::DescriptorSetLayout layout)
{
	desc_set_layout = layout;
	return *this;
}

std::shared_ptr<Texture> TextureCreator::create() const
{
	auto& ctx    = Context::get();
	auto& device = ctx.device;

	auto ptr = new Texture();

	ptr->image      = device.createImage(image_info);
	ptr->memory     = allocateMemory(ptr->image, ptr->allocated_size);
	ptr->image_view = createImageView(ptr->image);
	ptr->sampler    = createSampler();

	vk::DescriptorSetAllocateInfo desc_set_info = {};
	desc_set_info.descriptorPool     = ctx.descriptor_pool;
	desc_set_info.descriptorSetCount = 1;
	desc_set_info.pSetLayouts        = &desc_set_layout;

	ptr->desc_set = device.allocateDescriptorSets(desc_set_info).front();

	vk::DescriptorImageInfo desc_image_info = {};
	desc_image_info.sampler     = ptr->sampler;
	desc_image_info.imageView   = ptr->image_view;
	desc_image_info.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;

	vk::WriteDescriptorSet write_desc_set = {};
	write_desc_set.dstSet          = ptr->desc_set;
	write_desc_set.dstBinding      = 0;
	write_desc_set.dstArrayElement = 0;
	write_desc_set.descriptorCount = 1;
	write_desc_set.descriptorType  = vk::DescriptorType::eCombinedImageSampler;
	write_desc_set.pImageInfo      = &desc_image_info;

	device.updateDescriptorSets(1, &write_desc_set, 0, nullptr);

	ptr->extent = image_info.extent;
	ptr->format = image_info.format;

	return std::shared_ptr<Texture>(ptr);
}

vk::DeviceMemory TextureCreator::allocateMemory(vk::Image image, vk::DeviceSize& size) const
{
	auto& ctx    = Context::get();
	auto& device = ctx.device;

	auto req = device.getImageMemoryRequirements(image);

	vk::MemoryAllocateInfo image_info = {};
	image_info.allocationSize  = size = req.size;
	image_info.memoryTypeIndex = ctx.findMemoryType(req.memoryTypeBits, memory_props);

	auto memory = device.allocateMemory(image_info);

	device.bindImageMemory(image, memory, 0);

	return memory;
}

vk::ImageView TextureCreator::createImageView(vk::Image image) const
{
	auto& device = Context::get().device;

	vk::ImageViewCreateInfo info = {};
	info.image        = image;
	info.viewType     = to_image_view_type(image_info.imageType);
	info.format       = image_info.format;

	info.components.r = vk::ComponentSwizzle::eIdentity;
	info.components.g = vk::ComponentSwizzle::eIdentity;
	info.components.b = vk::ComponentSwizzle::eIdentity;
	info.components.a = vk::ComponentSwizzle::eIdentity;

	info.subresourceRange.aspectMask     = vk::ImageAspectFlagBits::eColor;
	info.subresourceRange.baseMipLevel   = 0;
	info.subresourceRange.levelCount     = image_info.mipLevels;
	info.subresourceRange.baseArrayLayer = 0;
	info.subresourceRange.layerCount     = 1;

	return device.createImageView(info);
}

vk::Sampler TextureCreator::createSampler() const
{
	auto& device = Context::get().device;

	return device.createSampler(sampler_info);
}

Texture::Texture() :
	memory(nullptr),
	image(nullptr),
	image_view(nullptr),
	sampler(nullptr),
	desc_set(nullptr),
	staging_buffer(vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible),
	extent(0, 0),
	allocated_size(0),
	format(vk::Format::eUndefined)
{
}

Texture::~Texture()
{
	auto& ctx    = Context::get();
	auto& device = ctx.device;

	device.free(ctx.descriptor_pool, 1, &desc_set);
	device.free(memory);
	device.destroy(image);
	device.destroy(image_view);
	device.destroy(sampler);
}

void Texture::update(void* pixels)
{
	auto& ctx    = Context::get();
	auto& device = ctx.device;

	size_t size = extent.width * extent.height * extent.depth * format_size_in_byte(format);

	staging_buffer.resize(size);
	auto* map = staging_buffer.map();
	memcpy(map, pixels, size);
	staging_buffer.flush();
	staging_buffer.unmap();

	auto cmd = ctx.beginSingleTimeCommmand();

	ctx.transitionImageLayout(
		cmd,
		image,
		vk::Format::eR8G8B8A8Unorm,
		vk::ImageLayout::eUndefined,
		vk::ImageLayout::eTransferDstOptimal);

	vk::BufferImageCopy image_copy = {
		0, 0, 0,
		{ vk::ImageAspectFlagBits::eColor, 0, 0, 1 },
		{ 0, 0, 0 },
		extent
	};

	cmd.copyBufferToImage(
		staging_buffer.getBuffer(),
		image,
		vk::ImageLayout::eTransferDstOptimal, 
		1, &image_copy);

	ctx.transitionImageLayout(
		cmd,
		image,
		vk::Format::eR8G8B8A8Unorm,
		vk::ImageLayout::eTransferDstOptimal,
		vk::ImageLayout::eShaderReadOnlyOptimal);

	ctx.endSingleTimeCommmand(cmd);
}

vk::DescriptorSet Texture::getDescriptorSet() const
{
	return desc_set;
}

VKDL_END