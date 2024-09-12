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

static vk::ImageView create_image_view(const vk::ImageCreateInfo& image_info, vk::Image image)
{
	auto& device = VKDL_NAMESPACE_NAME::Context::get().device;

	vk::ImageViewCreateInfo view_info = {};
	view_info.image    = image;
	view_info.viewType = to_image_view_type(image_info.imageType);
	view_info.format   = image_info.format;

	view_info.components.r = vk::ComponentSwizzle::eIdentity;
	view_info.components.g = vk::ComponentSwizzle::eIdentity;
	view_info.components.b = vk::ComponentSwizzle::eIdentity;
	view_info.components.a = vk::ComponentSwizzle::eIdentity;

	view_info.subresourceRange.aspectMask     = vk::ImageAspectFlagBits::eColor;
	view_info.subresourceRange.baseMipLevel   = 0;
	view_info.subresourceRange.levelCount     = image_info.mipLevels;
	view_info.subresourceRange.baseArrayLayer = 0;
	view_info.subresourceRange.layerCount     = 1;

	return device.createImageView(view_info);
}

VKDL_BEGIN

Texture::Texture(const TextureInfo& info) :
	info(info),
	image(nullptr),
	image_view(nullptr),
	memory(nullptr),
	sampler(nullptr),
	desc_set(nullptr),
	allocated_size(0),
	staging_buffer(vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible)
{
	auto& ctx    = Context::get();
	auto& device = ctx.device;

	image      = device.createImage(info.image_info);
	memory     = allocateMemory(image, allocated_size);
	image_view = create_image_view(info.image_info, image);
	sampler    = device.createSampler(info.sampler_info);
	desc_set   = createDescriptorSet(sampler, image_view);
}

Texture::Texture() :
	info(),
	image(nullptr),
	image_view(nullptr),
	memory(nullptr),
	sampler(nullptr),
	desc_set(nullptr),
	allocated_size(0),
	staging_buffer(vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible)
{
}

Texture::Texture(Texture&& rhs) :
	info(std::exchange(rhs.info, {})),
	image(std::exchange(rhs.image, nullptr)),
	image_view(std::exchange(rhs.image_view, nullptr)),
	memory(std::exchange(rhs.memory, nullptr)),
	sampler(std::exchange(rhs.sampler, nullptr)),
	desc_set(std::exchange(rhs.desc_set, nullptr)),
	staging_buffer(std::move(rhs.staging_buffer)),
	allocated_size(std::exchange(rhs.allocated_size, 0))
{
}

Texture::~Texture()
{
	clear();
}

Texture& Texture::operator=(Texture&& rhs)
{
	clear();

	info            = std::exchange(rhs.info, {});
	image           = std::exchange(rhs.image, nullptr);
	image_view      = std::exchange(rhs.image_view, nullptr);
	memory          = std::exchange(rhs.memory, nullptr);
	sampler         = std::exchange(rhs.sampler, nullptr);
	desc_set        = std::exchange(rhs.desc_set, nullptr);
	allocated_size  = std::exchange(rhs.allocated_size, 0);
	staging_buffer  = std::move(rhs.staging_buffer);

	return *this;
}

const vk::DescriptorSet& Texture::getDescriptorSet() const
{
	return desc_set;
}

void Texture::update(void* pixels)
{
	update(pixels, ivec2(0, 0), extent());
}

void Texture::update(void* pixels, const ivec2& offset, const uvec2& size)
{
	auto& ctx = Context::get();
	auto  device = ctx.device;
	auto  transfer_size = size.x * size.y * format_size_in_byte(info.image_info.format);

	staging_buffer.resize(transfer_size);
	memcpy(staging_buffer.map(), pixels, transfer_size);
	staging_buffer.flush();

	auto cmd = ctx.beginSingleTimeCommmand();

	ctx.transitionImageLayout(
		cmd,
		image,
		info.image_info.format,
		vk::ImageLayout::eUndefined,
		vk::ImageLayout::eTransferDstOptimal);

	vk::BufferImageCopy image_copy = {};
	image_copy.bufferOffset      = 0;
	image_copy.bufferRowLength   = 0;
	image_copy.bufferImageHeight = 0;
	image_copy.imageSubresource  = vk::ImageSubresourceLayers{ vk::ImageAspectFlagBits::eColor, info.image_info.mipLevels - 1, 0, 1 };
	image_copy.imageOffset       = vk::Offset3D{ offset.x, offset.y, 0 };
	image_copy.imageExtent       = vk::Extent3D{ size.x, size.y, 1 };

	cmd.copyBufferToImage(
		staging_buffer.getBuffer(),
		image,
		vk::ImageLayout::eTransferDstOptimal,
		1, &image_copy);

	ctx.transitionImageLayout(
		cmd,
		image,
		info.image_info.format,
		vk::ImageLayout::eTransferDstOptimal,
		vk::ImageLayout::eShaderReadOnlyOptimal);

	ctx.endSingleTimeCommmand(cmd);
}

void Texture::resize(uint32_t width, uint32_t height)
{
	VKDL_CHECK(!is_null());
	VKDL_CHECK(width != 0 && height != 0);

	if (info.image_info.extent.width == width && info.image_info.extent.height == height) return;

	auto& ctx    = Context::get();
	auto& device = ctx.device;

	info.image_info.extent.width  = width;
	info.image_info.extent.height = height;

	auto new_image      = device.createImage(info.image_info);
	auto new_memory     = allocateMemory(new_image, allocated_size);

	auto cmd = ctx.beginSingleTimeCommmand();

	ctx.transitionImageLayout(
		cmd,
		image,
		info.image_info.format,
		vk::ImageLayout::eUndefined,
		vk::ImageLayout::eTransferSrcOptimal);

	ctx.transitionImageLayout(
		cmd,
		new_image,
		info.image_info.format,
		vk::ImageLayout::eUndefined,
		vk::ImageLayout::eTransferDstOptimal);

	vk::ImageCopy image_copy = {};
	image_copy.srcSubresource = vk::ImageSubresourceLayers{ vk::ImageAspectFlagBits::eColor, info.image_info.mipLevels - 1, 0, 1 };
	image_copy.srcOffset      = vk::Offset3D{};
	image_copy.dstSubresource = vk::ImageSubresourceLayers{ vk::ImageAspectFlagBits::eColor, info.image_info.mipLevels - 1, 0, 1 };
	image_copy.dstOffset      = vk::Offset3D{};
	image_copy.extent         = info.image_info.extent;

	ctx.transitionImageLayout(
		cmd,
		new_image,
		info.image_info.format,
		vk::ImageLayout::eTransferDstOptimal,
		vk::ImageLayout::eShaderReadOnlyOptimal);

	ctx.endSingleTimeCommmand(cmd);

	device.destroy(std::exchange(image, new_image));
	device.destroy(std::exchange(image_view, create_image_view(info.image_info, new_image)));
	device.destroy(std::exchange(sampler, device.createSampler(info.sampler_info)));
	device.free(std::exchange(memory, new_memory));
	device.free(ctx.descriptor_pool, 1, &desc_set);
	desc_set = createDescriptorSet(sampler, image_view);
}

void Texture::clear()
{
	auto& ctx    = Context::get();
	auto  device = ctx.device;

	device.free(ctx.descriptor_pool, 1, &desc_set);
	device.free(std::exchange(memory, nullptr));
	device.destroy(std::exchange(image, nullptr));
	device.destroy(std::exchange(image_view, nullptr));
	device.destroy(std::exchange(sampler, nullptr));
	
	desc_set       = nullptr;
	allocated_size = 0;
}

TextureInfo Texture::getTextureInfo() const
{
	return info;
}

vk::Format Texture::format() const
{
	return info.image_info.format;
}

uvec2 Texture::extent() const
{
	return uvec2(info.image_info.extent.width, info.image_info.extent.height);
}

size_t Texture::size_in_bytes() const
{
	auto width  = info.image_info.extent.width;
	auto height = info.image_info.extent.height;

	return width * height * format_size_in_byte(info.image_info.format);
}

size_t Texture::capacity() const
{
	return allocated_size / format_size_in_byte(info.image_info.format);
}

bool Texture::is_null() const
{
	return (std::underlying_type_t<vk::ImageUsageFlagBits>)info.image_info.usage == 0;
}

bool Texture::empty() const
{
	return image != nullptr;
}

void Texture::swap(Texture& rhs)
{
	std::swap(info, rhs.info);
	std::swap(image, rhs.image);
	std::swap(image_view, rhs.image_view);
	std::swap(memory, rhs.memory);
	std::swap(sampler, rhs.sampler);
	std::swap(desc_set, rhs.desc_set);
	std::swap(allocated_size, rhs.allocated_size);
	staging_buffer.swap(rhs.staging_buffer);
}

vk::DeviceMemory Texture::allocateMemory(vk::Image image, vk::DeviceSize& size) const
{
	auto& ctx    = Context::get();
	auto& device = ctx.device;

	auto req = device.getImageMemoryRequirements(image);

	vk::MemoryAllocateInfo image_info = {};
	image_info.allocationSize  = size = req.size;
	image_info.memoryTypeIndex = ctx.findMemoryType(req.memoryTypeBits, info.memory_props);

	auto memory = device.allocateMemory(image_info);

	device.bindImageMemory(image, memory, 0);

	return memory;
}

vk::DescriptorSet Texture::createDescriptorSet(vk::Sampler sampler, vk::ImageView image_view) const
{
	auto& ctx    = Context::get();
	auto& device = ctx.device;

	vk::DescriptorSetAllocateInfo desc_set_info = {};
	desc_set_info.descriptorPool     = ctx.descriptor_pool;
	desc_set_info.descriptorSetCount = 1;
	desc_set_info.pSetLayouts        = &info.desc_set_layout;

	auto desc_set = device.allocateDescriptorSets(desc_set_info).front();

	vk::DescriptorImageInfo desc_image_info = {};
	desc_image_info.sampler     = sampler;
	desc_image_info.imageView   = image_view;
	desc_image_info.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;

	vk::WriteDescriptorSet write_desc_set = {};
	write_desc_set.dstSet          = desc_set;
	write_desc_set.dstBinding      = 0;
	write_desc_set.dstArrayElement = 0;
	write_desc_set.descriptorCount = 1;
	write_desc_set.descriptorType  = vk::DescriptorType::eCombinedImageSampler;
	write_desc_set.pImageInfo      = &desc_image_info;

	device.updateDescriptorSets(1, &write_desc_set, 0, nullptr);

	return desc_set;
}

TextureCreator::TextureCreator() :
	info()
{
	info.image_info.flags                 = {};
	info.image_info.imageType             =	{};
	info.image_info.format                =	{};
	info.image_info.extent                =	vk::Extent3D{};
	info.image_info.mipLevels             =	1;
	info.image_info.arrayLayers           =	1;
	info.image_info.samples               =	vk::SampleCountFlagBits::e1;
	info.image_info.tiling                =	{};
	info.image_info.usage                 =	{};
	info.image_info.sharingMode           =	vk::SharingMode::eExclusive;
	info.image_info.queueFamilyIndexCount =	0;
	info.image_info.pQueueFamilyIndices   =	nullptr;
	info.image_info.initialLayout         =	vk::ImageLayout::eUndefined;
	
	info.sampler_info.flags                   = {};
	info.sampler_info.magFilter               = vk::Filter::eLinear;
	info.sampler_info.minFilter               = vk::Filter::eLinear;
	info.sampler_info.mipmapMode              = vk::SamplerMipmapMode::eLinear;
	info.sampler_info.addressModeU            = vk::SamplerAddressMode::eRepeat;
	info.sampler_info.addressModeV            = vk::SamplerAddressMode::eRepeat;
	info.sampler_info.addressModeW            = vk::SamplerAddressMode::eRepeat;
	info.sampler_info.mipLodBias              = false;
	info.sampler_info.anisotropyEnable        = false;
	info.sampler_info.maxAnisotropy           = 1.f;
	info.sampler_info.compareEnable           = false;
	info.sampler_info.compareOp               = vk::CompareOp::eNever;
	info.sampler_info.minLod                  = -1000.f;
	info.sampler_info.maxLod                  = 1000.f;
	info.sampler_info.borderColor             = vk::BorderColor::eFloatTransparentBlack;
	info.sampler_info.unnormalizedCoordinates = false;

	info.memory_props = vk::MemoryPropertyFlagBits::eDeviceLocal;
}

TextureCreator& TextureCreator::setImageFormat(vk::Format format)
{
	info.image_info.format = format;
	return *this;
}

TextureCreator& TextureCreator::setImageExtent(uint32_t width, uint32_t height)
{
	info.image_info.extent    = vk::Extent3D{ width, height, 1 };
	info.image_info.imageType = vk::ImageType::e2D;
	return *this;
}

TextureCreator& TextureCreator::setImageExtent(vk::Extent2D extent)
{
	info.image_info.extent    = vk::Extent3D{ extent, 1 };
	info.image_info.imageType = vk::ImageType::e2D;
	return *this;
}

TextureCreator& TextureCreator::setImageMipLevels(uint32_t mip_levels)
{
	info.image_info.mipLevels = mip_levels;
	return *this;
}

TextureCreator& TextureCreator::setImageSamples(vk::SampleCountFlagBits samples)
{
	info.image_info.samples = samples;
	return *this;
}

TextureCreator& TextureCreator::setImageTiling(vk::ImageTiling tiling)
{
	info.image_info.tiling = tiling;
	return *this;
}

TextureCreator& TextureCreator::setImageUsage(vk::ImageUsageFlags usage)
{
	info.image_info.usage = usage;
	return *this;
}

TextureCreator& TextureCreator::setImageSharingMode(vk::SharingMode mode)
{
	info.image_info.sharingMode = mode;
	return *this;
}

TextureCreator& TextureCreator::setMemoryProperties(vk::MemoryPropertyFlags props)
{
	info.memory_props = props;
	return *this;
}

TextureCreator& TextureCreator::setSamplerMagFilter(vk::Filter filter)
{
	info.sampler_info.magFilter = filter;
	return *this;
}

TextureCreator& TextureCreator::setSamplerMinFilter(vk::Filter filter)
{
	info.sampler_info.minFilter = filter;
	return *this;
}

TextureCreator& TextureCreator::setSamplerMipMapMode(vk::SamplerMipmapMode mode)
{
	info.sampler_info.mipmapMode = mode;
	return *this;
}

TextureCreator& TextureCreator::setSamplerAddressMode(vk::SamplerAddressMode U, vk::SamplerAddressMode V, vk::SamplerAddressMode W)
{
	info.sampler_info.addressModeU = U;
	info.sampler_info.addressModeV = V;
	info.sampler_info.addressModeW = W;
	return *this;
}

TextureCreator& TextureCreator::setSamplerMipLodBias(bool value)
{
	info.sampler_info.mipLodBias = value;
	return *this;
}

TextureCreator& TextureCreator::setSamplerAnisotropy(float max_anisotropy)
{
	info.sampler_info.anisotropyEnable = true;
	info.sampler_info.maxAnisotropy    = max_anisotropy;
	return *this;
}

TextureCreator& TextureCreator::setSamplerCompare(vk::CompareOp op)
{
	info.sampler_info.compareEnable = true;
	info.sampler_info.compareOp     = op;
	return *this;
}

TextureCreator& TextureCreator::setSamplerLod(float min, float max)
{
	info.sampler_info.minLod = min;
	info.sampler_info.maxLod = max;
	return *this;
}

TextureCreator& TextureCreator::setSamplerBorderColor(vk::BorderColor color)
{
	info.sampler_info.borderColor = color;
	return *this;
}

TextureCreator& TextureCreator::setSamplerUnnormalizedCoordinates(bool value)
{
	info.sampler_info.unnormalizedCoordinates = value;
	return *this;
}

TextureCreator& TextureCreator::setDescriptorSetLayout(vk::DescriptorSetLayout layout)
{
	info.desc_set_layout = layout;
	return *this;
}

Texture TextureCreator::create() const
{
	return Texture(info);
}

VKDL_END