#pragma once

#include "include_vulkan.h"
#include "context.h"
#include "exception.h"

VKDL_BEGIN
VKDL_PRIV_BEGIN

template <class Iter>
using iter_cat_t = typename std::iterator_traits<Iter>::iterator_category;

template <class T, class = void>
constexpr bool is_iterator_v = false;

template <class T>
constexpr bool is_iterator_v<T, std::void_t<iter_cat_t<T>>> = true;

VKDL_PRIV_END

template <class T>
class Buffer
{
public:
	static VKDL_NODISCARD VKDL_INLINE Buffer<T> createVertexBuffer(size_t size)
	{
		return Buffer<T>(
			size,
			vk::BufferUsageFlagBits::eVertexBuffer,
			vk::MemoryPropertyFlagBits::eDeviceLocal
			| vk::MemoryPropertyFlagBits::eHostVisible);
	}

	static VKDL_NODISCARD VKDL_INLINE Buffer<T> createIndexBuffer(size_t size)
	{
		return Buffer<T>(
			size,
			vk::BufferUsageFlagBits::eIndexBuffer,
			vk::MemoryPropertyFlagBits::eDeviceLocal
			| vk::MemoryPropertyFlagBits::eHostVisible);
	}

public:
	VKDL_INLINE Buffer() VKDL_NOEXCEPT :
		buffer(nullptr),
		memory(nullptr),
		mapped_ptr(nullptr),
		allocated_size(0),
		item_size(0),
		usage(),
		memory_props(),
		sharing_mode(vk::SharingMode::eExclusive)
	{}
	
	VKDL_INLINE Buffer(vk::BufferUsageFlags usage, vk::MemoryPropertyFlags memory_props, vk::SharingMode sharing_mode = vk::SharingMode::eExclusive) VKDL_NOEXCEPT :
		buffer(nullptr),
		memory(nullptr),
		mapped_ptr(nullptr),
		allocated_size(0),
		item_size(0),
		usage(usage),
		memory_props(memory_props),
		sharing_mode(sharing_mode)
	{}
	
	VKDL_INLINE Buffer(size_t size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags memory_props, vk::SharingMode sharing_mode = vk::SharingMode::eExclusive) :
		Buffer(usage, memory_props, sharing_mode)
	{
		resize(size);
	}

	template <class Iter, std::enable_if_t<VKDL_PRIV_NAMESPACE_NAME::is_iterator_v<Iter>, int> = 0>
	VKDL_INLINE Buffer(Iter first, Iter last, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags memory_props, vk::SharingMode sharing_mode = vk::SharingMode::eExclusive) :
		Buffer(std::distance(first, last), usage, memory_props, sharing_mode)
	{
		std::copy(first, last, map());
		unmap(true);
	}

	VKDL_INLINE Buffer(const Buffer& rhs) :
		Buffer(rhs.item_size, rhs.usage, rhs.memory_props, rhs.sharing_mode)
	{
		copyBuffer(buffer, rhs.buffer, (vk::DeviceSize)size_in_bytes());
	}
	
	VKDL_INLINE Buffer(Buffer&& rhs) VKDL_NOEXCEPT :
		buffer(std::exchange(rhs.buffer, nullptr)),
		memory(std::exchange(rhs.memory, nullptr)),
		mapped_ptr(std::exchange(rhs.mapped_ptr, nullptr)),
		allocated_size(std::exchange(rhs.allocated_size, 0)),
		item_size(std::exchange(rhs.item_size, 0)),
		usage(std::exchange(rhs.usage, {})),
		memory_props(std::exchange(rhs.memory_props, {})),
		sharing_mode(std::exchange(rhs.sharing_mode, vk::SharingMode::eExclusive))
	{}
	
	VKDL_INLINE ~Buffer()
	{
		clear();
	}

	VKDL_INLINE void init(vk::BufferUsageFlags usage, vk::MemoryPropertyFlags memory_props, vk::SharingMode sharing_mode = vk::SharingMode::eExclusive)
	{
		if (!empty()) clear();

		this->usage        = usage;
		this->memory_props = memory_props;
		this->sharing_mode = sharing_mode;
	}

	VKDL_INLINE Buffer& operator=(const Buffer& rhs)
	{
		init(rhs.usage, rhs.memory_props, rhs.sharing_mode);
		resize(rhs.item_size);
		copyBuffer(buffer, rhs.buffer, (vk::DeviceSize)size_in_bytes());
	}

	VKDL_INLINE Buffer& operator=(Buffer&& rhs) VKDL_NOEXCEPT
	{
		clear();

		buffer         = std::exchange(rhs.buffer, nullptr);
		memory         = std::exchange(rhs.memory, nullptr);
		mapped_ptr     = std::exchange(rhs.mapped_ptr, nullptr);
		allocated_size = std::exchange(rhs.allocated_size, 0);
		item_size      = std::exchange(rhs.item_size, 0);
		usage          = std::exchange(rhs.usage, {});
		memory_props   = std::exchange(rhs.memory_props, {});
		sharing_mode   = std::exchange(rhs.sharing_mode, vk::SharingMode::eExclusive);

		return *this;
	}

	VKDL_NODISCARD VKDL_INLINE const vk::Buffer& getBuffer() const
	{
		return buffer;
	}

	VKDL_NODISCARD VKDL_INLINE T* map(vk::DeviceSize offset = 0, vk::DeviceSize size = VK_WHOLE_SIZE)
	{
		if (mapped_ptr) 
			return mapped_ptr;
		
		return mapped_ptr = static_cast<T*>(Context::get().device.mapMemory(memory, offset, size));
	}

	VKDL_INLINE void unmap(bool flush = false)
	{
		if (flush) this->flush();
		Context::get().device.unmapMemory(memory);
		mapped_ptr = nullptr;
	}

	VKDL_INLINE void flush(vk::DeviceSize offset = 0, vk::DeviceSize size = VK_WHOLE_SIZE)
	{
		vk::MappedMemoryRange range = {};
		range.memory = memory;
		range.offset = offset;
		range.size   = size;

		VK_CHECK(Context::get().device.flushMappedMemoryRanges(1, &range));
	}

	VKDL_INLINE void resize(size_t new_size)
	{
		VKDL_CHECK(usage != vk::BufferUsageFlags(0) && memory_props != vk::MemoryPropertyFlags(0));

		if (new_size == 0) {
			clear();
			return;
		} else if (item_size == new_size) {
			return;
		}

		auto& ctx    = Context::get();
		auto  device = ctx.device;

		vk::BufferCreateInfo buffer_info = {};
		buffer_info.size        = sizeof(T) * new_size;
		buffer_info.usage       = usage;
		buffer_info.sharingMode = sharing_mode;

		auto new_buffer = device.createBuffer(buffer_info);
		auto req        = device.getBufferMemoryRequirements(new_buffer);

		if (allocated_size < req.size) {
			vk::MemoryAllocateInfo image_info = {};
			image_info.allocationSize  = req.size;
			image_info.memoryTypeIndex = ctx.findMemoryType(req.memoryTypeBits, memory_props);

			auto new_memory = device.allocateMemory(image_info);
			device.bindBufferMemory(new_buffer, new_memory, 0);

			if (memory != nullptr) {
				copyBuffer(new_buffer, buffer, (vk::DeviceSize)size_in_bytes());
				device.free(std::exchange(memory, new_memory));
			} else {
				memory = new_memory;
			}

			allocated_size = req.size;
			mapped_ptr     = nullptr;
		} else {
			device.bindBufferMemory(new_buffer, memory, 0);
		}

		device.destroy(std::exchange(buffer, new_buffer));
		item_size  = new_size;
	}

	VKDL_INLINE void clear()
	{
		auto device = Context::get().device;
		device.destroy(std::exchange(buffer, nullptr));
		device.free(std::exchange(memory, nullptr));
		mapped_ptr     = nullptr;
		allocated_size = 0;
		item_size      = 0;
	}

	VKDL_NODISCARD VKDL_INLINE size_t size() const VKDL_NOEXCEPT
	{
		return item_size;
	}

	VKDL_NODISCARD VKDL_INLINE size_t size_in_bytes() const VKDL_NOEXCEPT
	{
		return sizeof(T) * item_size;
	}

	VKDL_NODISCARD VKDL_INLINE size_t capacity() const VKDL_NOEXCEPT
	{
		return allocated_size / sizeof(T);
	}

	VKDL_NODISCARD VKDL_INLINE bool is_null() const VKDL_NOEXCEPT
	{
		return (std::underlying_type_t<vk::MemoryAllocateFlagBits>)memory_props == 0;
	}

	VKDL_NODISCARD VKDL_INLINE bool empty() const VKDL_NOEXCEPT
	{
		return buffer == nullptr;
	}

	VKDL_INLINE void swap(Buffer<T>& rhs) VKDL_NOEXCEPT
	{
		std::swap(buffer, rhs.buffer);
		std::swap(memory, rhs.memory);
		std::swap(mapped_ptr, rhs.mapped_ptr);
		std::swap(allocated_size, rhs.allocated_size);
		std::swap(item_size, rhs.item_size);
		std::swap(usage, rhs.usage);
		std::swap(memory_props, rhs.memory_props);
		std::swap(sharing_mode, rhs.sharing_mode);
	}

private:
	VKDL_INLINE void copyBuffer(vk::Buffer dst_buffer, vk::Buffer src_buffer, vk::DeviceSize size)
	{
		auto& ctx = Context::get();
		auto  cmd = ctx.beginSingleTimeCommmand();

		vk::BufferCopy region = {};
		region.srcOffset = 0;
		region.dstOffset = 0;
		region.size      = size;
		cmd.copyBuffer(src_buffer, dst_buffer, 1, &region);

		ctx.endSingleTimeCommmand(cmd);
	}

protected:
	vk::Buffer              buffer;
	vk::DeviceMemory        memory;
	T*                      mapped_ptr;
	vk::DeviceSize          allocated_size;
	size_t                  item_size;
	vk::BufferUsageFlags    usage;
	vk::MemoryPropertyFlags memory_props;
	vk::SharingMode         sharing_mode;
};

VKDL_END