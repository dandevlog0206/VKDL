#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x3.hpp>
#include <glm/mat4x4.hpp>
#include "../core/config.h"

VKDL_BEGIN

template <class T>
using vec2_base = glm::vec<2, T, glm::defaultp>;

typedef glm::vec2  vec2;
typedef glm::ivec2 ivec2;
typedef glm::uvec2 uvec2;

typedef glm::vec3  vec3;
typedef glm::ivec3 ivec3;
typedef glm::uvec3 uvec3;

typedef glm::vec4  vec4;
typedef glm::ivec4 ivec4;
typedef glm::uvec4 uvec4;

typedef glm::mat3x4 mat3x4;
typedef glm::mat4x4 mat4x4;

VKDL_END