#pragma once

#include "vector_type.h"

VKDL_BEGIN

template <class T>
struct _Rect_vector { using type = void; };

template <>
struct _Rect_vector<float> { using type = vec2; };

template <>
struct _Rect_vector<int32_t> { using type = ivec2; };

template <>
struct _Rect_vector<uint32_t> { using type = uvec2; };

template <class T>
struct RectBase {
	using vector_type = typename _Rect_vector<T>::type;

	RectBase() = default;
	RectBase(const RectBase&) = default;
	RectBase(RectBase&&) = default;

	RectBase(const T& pos_x, const T& pos_y, const T& width, const T& height) :
		position(pos_x, pos_y),
		size(width, height) 
	{}

	RectBase(const vector_type& pos, const vector_type& size) :
		position(pos),
		size(size)
	{}

	template <class U>
	RectBase(const RectBase<U>& rhs) VKDL_NOEXCEPT :
	position(static_cast<T>(rhs.position.x), static_cast<T>(rhs.position.y)),
	size(static_cast<T>(rhs.size.x), static_cast<T>(rhs.size.y))
	{}

	RectBase& operator=(const RectBase&) = default;
	RectBase& operator=(RectBase&&) = default;

	vector_type center() const VKDL_NOEXCEPT
	{
		return {
			position.x + size.x / static_cast<T>(2.f),
			position.y + size.y / static_cast<T>(2.f),
		};
	}

	float area() const VKDL_NOEXCEPT
	{
		return size.x * size.y;
	}

	bool contain(const vector_type& pos) const VKDL_NOEXCEPT
	{
		return position.x <= pos.x && pos.x <= position.x + size.x && position.y <= pos.y && pos.y <= position.y + size.y;
	}

	vec2_base<T> position;
	vec2_base<T> size;
};

using rect  = RectBase<float>;
using irect = RectBase<int32_t>;
using urect = RectBase<uint32_t>;

VKDL_END