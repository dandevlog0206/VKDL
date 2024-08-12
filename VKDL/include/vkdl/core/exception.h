#pragma once

#include <exception>
#include <stdexcept>
#include <assert.h>

#define VKDL_ERROR(msg) throw std::runtime_error(msg)
#define VKDL_CHECK(expression) while (expression) { throw std::runtime_error("error"); }
#define VKDL_CHECK_MSG(expression, msg) while (expression) { throw std::runtime_error(msg); }
#define VKDL_ASSERT(expression) assert(expression)
#define VKDL_ASSERT_MSG(expression, msg) assert((expression) && msg)