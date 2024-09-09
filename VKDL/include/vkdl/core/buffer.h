#pragma once

#include "include_vulkan.h"
#include "context.h"
#include "exception.h"

VKDL_BEGIN

template <class T>
class Buffer
{
public:
	Buffer();
	Buffer(vk::BufferUsageFlags usage, vk::MemoryPropertyFlags memory_props, vk::SharingMode sharing_mode = vk::SharingMode::eExclusive);
	Buffer(size_t size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags memory_props, vk::SharingMode sharing_mode = vk::SharingMode::eExclusive);
	Buffer(const Buffer& rhs);
	Buffer(Buffer&& rhs);
	~Buffer();

	void init(vk::BufferUsageFlags usage, vk::MemoryPropertyFlags memory_props, vk::SharingMode sharing_mode = vk::SharingMode::eExclusive);

	const vk::Buffer& getBuffer() const;
	vk::Buffer& getBuffer();

	size_t size() const;
	size_t size_in_byte() const;
	size_t capacity() const;

	T* map();
	void unmap();
	void flush();

	void resize(size_t size);

	void clear();

	bool empty() const;

protected:
	vk::Buffer              buffer;
	vk::DeviceMemory        memory;
	vk::BufferUsageFlags    usage;
	vk::MemoryPropertyFlags memory_props;
	vk::SharingMode         sharing_mode;
	size_t                  actual_size;
	size_t                  allocated_size;
};

template<class T>
Buffer<T>::Buffer() :
	buffer(nullptr),
	memory(nullptr),
	usage(),
	memory_props(),
	sharing_mode(vk::SharingMode::eExclusive),
	actual_size(0),
	allocated_size(0)
{
}

template<class T>
Buffer<T>::Buffer(vk::BufferUsageFlags usage, vk::MemoryPropertyFlags memory_props, vk::SharingMode sharing_mode) :
	buffer(nullptr),
	memory(nullptr),
	usage(usage),
	memory_props(memory_props),
	sharing_mode(sharing_mode),
	actual_size(0),
	allocated_size(0)
{
}

template<class T>
Buffer<T>::Buffer(size_t size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags memory_props, vk::SharingMode sharing_mode) :
	Buffer(usage, memory_props, sharing_mode)
{
	resize(size);
}

template<class T>
Buffer<T>::Buffer(const Buffer& rhs) :
	Buffer(rhs.usage, rhs.memory_props, rhs.sharing_mode)
{
	auto& ctx = Context::get();

	resize(rhs.actual_size);
	
	auto cmd = ctx.beginSingleTimeCommmand();

	vk::BufferCopy region = {};
	region.srcOffset = 0;
	region.dstOffset = 0;
	region.size      = sizeof(T) * actual_size;
	cmd.copyBuffer(rhs.buffer, buffer, 1, &region);

	ctx.endSingleTimeCommmand(cmd);
}

template<class T>
Buffer<T>::Buffer(Buffer&& rhs) :
	buffer(std::exchange(rhs.buffer, nullptr)),
	memory(std::exchange(rhs.memory, nullptr)),
	usage(std::exchange(rhs.usage, {})),
	sharing_mode(std::exchange(rhs.sharing_mode, vk::SharingMode::eExclusive)),
	actual_size(std::exchange(rhs.actual_size, 0)),
	allocated_size(std::exchange(rhs.allocated_size, 0))
{
}

template<class T>
Buffer<T>::~Buffer()
{
	auto& device = Context::get().device;
	device.destroy(buffer);
	device.free(memory);
}

template<class T>
void Buffer<T>::init(vk::BufferUsageFlags usage, vk::MemoryPropertyFlags memory_props, vk::SharingMode sharing_mode)
{
	VKDL_CHECK(usage != vk::BufferUsageFlags(0) && memory_props != vk::MemoryPropertyFlags(0));
	
	clear();

	this->usage        = usage;
	this->memory_props = memory_props;
	this->sharing_mode = sharing_mode;
}

template<class T>
const vk::Buffer& Buffer<T>::getBuffer() const
{
	return buffer;
}

template<class T>
vk::Buffer& Buffer<T>::getBuffer()
{
	return buffer;
}

template<class T>
size_t Buffer<T>::size() const
{
	return actual_size;
}

template<class T>
size_t Buffer<T>::size_in_byte() const
{
	return sizeof(T) * actual_size;
}

template<class T>
size_t Buffer<T>::capacity() const
{
	return allocated_size / sizeof(T);
}

template<class T>
T* Buffer<T>::map()
{
	auto& device = Context::get().device;
	return (T*)device.mapMemory(memory, 0, VK_WHOLE_SIZE);
}

template<class T>
void Buffer<T>::unmap()
{
	auto& device = Context::get().device;
	device.unmapMemory(memory);
}

template<class T>
void Buffer<T>::flush()
{
	auto& device = Context::get().device;

	vk::MappedMemoryRange range = {};
	range.memory = memory;
	range.offset = 0;
	range.size   = VK_WHOLE_SIZE;

	VK_CHECK(device.flushMappedMemoryRanges(1, &range));
}

template<class T>
void Buffer<T>::resize(size_t new_size)
{
	VKDL_CHECK(usage != vk::BufferUsageFlags(0) && memory_props != vk::MemoryPropertyFlags(0));

	if (actual_size == new_size) return;

	auto& ctx    = Context::get();
	auto& device = ctx.device;

	{
		device.destroy(buffer);

		vk::BufferCreateInfo image_info = {};
		image_info.size        = sizeof(T) * new_size;
		image_info.usage       = usage;
		image_info.sharingMode = sharing_mode;
		
		buffer      = device.createBuffer(image_info);
		actual_size = new_size;
	}

	auto req = device.getBufferMemoryRequirements(buffer);

	if (allocated_size < req.size) {
		device.free(memory);

		vk::MemoryAllocateInfo image_info = {};
		image_info.allocationSize  = req.size;
		image_info.memoryTypeIndex = ctx.findMemoryType(req.memoryTypeBits, memory_props);
		
		memory         = device.allocateMemory(image_info);
		allocated_size = req.size;
	}

	device.bindBufferMemory(buffer, memory, 0);
}

template<class T>
void Buffer<T>::clear()
{
	auto& device = Context::get().device;

	device.waitIdle();
	device.destroy(std::exchange(buffer, nullptr));
	device.free(std::exchange(memory, nullptr));
	actual_size    = 0;
	allocated_size = 0;
}

template<class T>
bool Buffer<T>::empty() const
{
	return buffer == nullptr;
}

VKDL_END