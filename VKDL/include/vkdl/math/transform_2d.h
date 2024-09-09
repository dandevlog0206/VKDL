#pragma once

#include "vector_type.h"
#include "../util/radian.h"

VKDL_BEGIN

class Transform2D
{
public:
	static Transform2D identity();

	Transform2D();
	Transform2D(float a00, float a01, float a02,
		float a10, float a11, float a12,
		float a20, float a21, float a22);

	Transform2D& translate(float x, float y);
	Transform2D& translate(const vec2& offset);

	Transform2D& rotate(radian rad);
	Transform2D& rotate(radian rad, float center_x, float center_y);
	Transform2D& rotate(radian rad, const vec2& center);

	Transform2D& scale(float scale);
	Transform2D& scale(float scale_x, float scale_y);
	Transform2D& scale(const vec2& scale);

	Transform2D& clear();

	Transform2D& operator=(const Transform2D& rhs);
	Transform2D& operator*=(const Transform2D& rhs);
	Transform2D operator*(const Transform2D& rhs) const;
	vec2 operator*(const vec2& rhs) const;

	bool operator==(const Transform2D& rhs) const;
	bool operator!=(const Transform2D& rhs) const;

private:
	mat3x4 mat;
};

VKDL_END