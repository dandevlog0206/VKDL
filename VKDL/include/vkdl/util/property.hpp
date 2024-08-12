/*************************************************************************/
/* property.hpp                                                          */
/*************************************************************************/
/* https://www.dandevlog.com/all/programming/1347/                       */
/* https://github.com/dandevlog0206/cpp-property                         */
/*************************************************************************/
/* Copyright (c) 2024 www.dandevlog.com                                  */
/*                                                                       */
/* Permission is hereby granted, free of charge, to any person obtaining */
/* a copy of this software and associated documentation files (the       */
/* "Software"), to deal in the Software without restriction, including   */
/* without limitation the rights to use, copy, modify, merge, publish,   */
/* distribute, sublicense, and/or sell copies of the Software, and to    */
/* permit persons to whom the Software is furnished to do so, subject to */
/* the following conditions:                                             */
/*                                                                       */
/* The above copyright notice and this permission notice shall be        */
/* included in all copies or substantial portions of the Software.       */
/*                                                                       */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,       */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF    */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.*/
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY  */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,  */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE     */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                */
/*************************************************************************/

#pragma once

#include <type_traits>
#include <stddef.h>
#include <stdint.h>

template <class T>
class __non_copyable_but {
	friend T;

	__non_copyable_but(const __non_copyable_but<T>&) = default;
	__non_copyable_but(__non_copyable_but<T>&&) = default;
	__non_copyable_but& operator=(const __non_copyable_but<T>&) = default;
	__non_copyable_but& operator=(__non_copyable_but<T>&&) = default;
};

#define __safe_offsetof(type, member) ((size_t)(std::addressof(((type*)0)->member)))

#define __PROPERTY_MAKE_NAME(__prop_type, __prop_name) __PROPERTY_##__prop_name

#define __PROPERTY_GET_ADDRESS_OF(__prop_name) (std::intptr_t)(this) - __safe_offsetof(__this_class_type, __prop_name)

#define __PROPERTY_GET_IMPL(__prop_name)                                                                \
			prop_type get() const {                                                                     \
				auto address = __PROPERTY_GET_ADDRESS_OF(__prop_name);                                  \
				return reinterpret_cast<__this_class_type*>(address)->__property_##__prop_name##_get(); \
			}                                                                                           \
			operator prop_type() const { return this->get(); }

#define __PROPERTY_SET_IMPL(__prop_name)                                                              \
			void set(const prop_type& value) {                                                        \
				auto address = __PROPERTY_GET_ADDRESS_OF(__prop_name);                                \
				reinterpret_cast<__this_class_type*>(address)->__property_##__prop_name##_set(value); \
			}                                                                                         \
			void operator=(const prop_type& value) { this->set(value); }

#define __PROPERTY_DEFAULT_IMPL(__prop_type, __prop_name, __get_impl, __set_impl)        \
	};                                                                                   \
	struct __PROPERTY_MAKE_NAME(__prop_type, __prop_name) {                              \
		using prop_type = __prop_type;                                                   \
		using this_type = __PROPERTY_MAKE_NAME(__prop_type, __prop_name);                \
		__PROPERTY_MAKE_NAME(__prop_type, __prop_name)() = default;                      \
		__PROPERTY_MAKE_NAME(__prop_type, __prop_name)(const this_type&) = default;      \
		__PROPERTY_MAKE_NAME(__prop_type, __prop_name)(const prop_type& T) : value(T) {} \
		this_type& operator=(const this_type&) = delete;                                 \
		this_type& operator=(this_type&&) = delete;                                      \
		__get_impl                                                                       \
		__set_impl                                                                       \
	private:                                                                             \
		prop_type value;                                                                 \
	};                                                                                   \
	union {                                                                              \
		__PROPERTY_MAKE_NAME(__prop_type, __prop_name) __prop_name

#define __PROPERTY_DEFAULT_GET_IMPL                    \
	operator prop_type() const { return value; }       \
	const prop_type& get_ref() const { return value; } \
	prop_type& get_ref() { return value; }             \
	prop_type get() const { return value; }

#define __PROPERTY_DEFAULT_SET_IMPL                                 \
	void operator=(const prop_type& value) { this->value = value; } \
	void set(const prop_type& value) { this->value = value; }

#define PROPERTY_INIT(__class_name)                          \
	using __this_class_type = __class_name;                  \
	const auto* __property_get_this() const { return this; } \
	auto* __property_get_this() { return this; }

#define PROPERTY union

#define PROPERTY_GET(__prop_type, __prop_name)                  \
		struct : public __non_copyable_but<__this_class_type> { \
			using prop_type = __prop_type;                      \
			__PROPERTY_GET_IMPL(__prop_name)                    \
		} __prop_name

#define PROPERTY_SET(__prop_type, __prop_name)                  \
		struct : public __non_copyable_but<__this_class_type> { \
			using prop_type = __prop_type;                      \
			__PROPERTY_SET_IMPL(__prop_name)                    \
		} __prop_name

#define PROPERTY_GET_SET(__prop_type, __prop_name)              \
		struct : public __non_copyable_but<__this_class_type> { \
			using prop_type = __prop_type;                      \
			__PROPERTY_GET_IMPL(__prop_name)                    \
			__PROPERTY_SET_IMPL(__prop_name)                    \
		} __prop_name

#define PROPERTY_DEFAULT_GET(__prop_type, __prop_name) \
	__PROPERTY_DEFAULT_IMPL(__prop_type, __prop_name,  \
		__PROPERTY_DEFAULT_GET_IMPL,)

#define PROPERTY_DEFAULT_SET(__prop_type, __prop_name) \
	__PROPERTY_DEFAULT_IMPL(__prop_type, __prop_name,, \
		__PROPERTY_DEFAULT_SET_IMPL)

#define PROPERTY_DEFAULT_GET_SET(__prop_type, __prop_name) \
	__PROPERTY_DEFAULT_IMPL(__prop_type, __prop_name,      \
		__PROPERTY_DEFAULT_GET_IMPL,                       \
		__PROPERTY_DEFAULT_SET_IMPL)

#define PROPERTY_DECL_GET(__prop_name) \
	decltype(__prop_name)::prop_type __property_##__prop_name##_get() const

#define PROPERTY_DECL_SET(__prop_name) \
	void __property_##__prop_name##_set(const decltype(__prop_name)::prop_type& value)

#define PROPERTY_IMPL_GET(__class_name, __prop_name) \
	decltype(__class_name::__prop_name)::prop_type __class_name::__property_##__prop_name##_get() const

#define PROPERTY_IMPL_SET(__class_name, __prop_name) \
	void __class_name::__property_##__prop_name##_set(const decltype(__class_name::__prop_name)::prop_type& value)
