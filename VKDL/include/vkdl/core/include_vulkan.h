#pragma once

#include "config.h"
#include "exception.h"

#ifdef VKDL_USE_PLATFORM
#ifdef VKDL_PLATFORM_WINDOWS
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#define VK_USE_PLATFORM_WIN32_KHR
#endif
#endif

#include <vulkan/vulkan.hpp>

#ifdef _DEBUG
// #define VKDL_CHECK(expression) VK2D_ASSERT((expression) == vk::Result::eSuccess)
#define VK_CHECK(expression) (void)expression
#else
#define VK_CHECK(expression) (void)expression
#endif