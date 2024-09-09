#pragma once

#include "color.h"

VKDL_BEGIN

struct Vertex3D
{
	vec3  pos;
	vec2  uv;
	Color col;
};

struct Vertex2D
{
	Vertex2D() :
		pos(),
		uv(0.5f, 0.5f),
		col()
	{}

	Vertex2D(const vec2& pos, const Color& col) :
		pos(pos),
		uv(0.5f, 0.5f),
		col(col)
	{}

	Vertex2D(const vec2& pos, const vec2& uv) :
		pos(pos),
		uv(uv),
		col(Colors::White)
	{}

	Vertex2D(const vec2& pos, const vec2& uv, const Color& col) :
		pos(pos),
		uv(uv),
		col(col)
	{}

	vec2  pos;
	vec2  uv;
	Color col;
};

VKDL_END