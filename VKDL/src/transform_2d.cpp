#include "../include/vkdl/math/transform_2d.h"

VKDL_BEGIN

Transform2D Transform2D::identity()
{
	return Transform2D();
}

Transform2D::Transform2D() :
	mat(1.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f)
{
}

Transform2D::Transform2D(float a00, float a01, float a02, float a10, float a11, float a12, float a20, float a21, float a22) :
	mat(a00, a10, a20, 0.f, a01, a11, a21, 0.f, a02, a12, a22, 0.f)
{
}

Transform2D& Transform2D::translate(float x, float y)
{
	mat[2][0] += mat[0][0] * x + mat[1][0] * y;
	mat[2][1] += mat[0][1] * x + mat[1][1] * y;
	mat[2][2] += mat[0][2] * x + mat[1][2] * y;

	return *this;
}

Transform2D& Transform2D::translate(const vec2& offset)
{
	return Transform2D::translate(offset.x, offset.y);
}

Transform2D& Transform2D::rotate(radian rad)
{
	float cos = std::cos(rad);
	float sin = std::sin(rad);

	*this *= Transform2D(
		cos, -sin, 0.f,
		sin, cos, 0.f,
		0.f, 0.f, 1.f);

	return *this;
}

Transform2D& Transform2D::rotate(radian rad, float center_x, float center_y)
{
	float cos = std::cos(rad);
	float sin = std::sin(rad);

	*this *= Transform2D(
		cos, -sin, center_x * (1 - cos) + center_y * sin,
		sin, cos, center_y * (1 - cos) - center_x * sin,
		0.f, 0.f, 1.f
	);

	return *this;
}

Transform2D& Transform2D::rotate(radian rad, const vec2& center)
{
	return Transform2D::rotate(rad, center.x, center.y);
}

Transform2D& Transform2D::scale(float scale)
{
	mat[0][0] *= scale;
	mat[0][1] *= scale;
	mat[0][2] *= scale;
	mat[1][0] *= scale;
	mat[1][1] *= scale;
	mat[1][2] *= scale;

	return *this;
}

Transform2D& Transform2D::scale(float scale_x, float scale_y)
{
	mat[0][0] *= scale_x;
	mat[0][1] *= scale_x;
	mat[0][2] *= scale_x;
	mat[1][0] *= scale_y;
	mat[1][1] *= scale_y;
	mat[1][2] *= scale_y;

	return *this;
}

Transform2D& Transform2D::scale(const vec2& scale)
{
	return Transform2D::scale(scale.x, scale.y);
}

Transform2D& Transform2D::clear()
{
	mat = mat3x4(1.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f);
	return *this;
}

Transform2D& Transform2D::operator=(const Transform2D& rhs)
{
	mat = rhs.mat;
	return *this;
}

Transform2D& Transform2D::operator*=(const Transform2D& rhs)
{
	mat3x4 copy = mat;

	mat[0][0] = copy[0][0] * rhs.mat[0][0] + copy[1][0] * rhs.mat[0][1] + copy[2][0] * rhs.mat[0][2];
	mat[1][0] = copy[0][0] * rhs.mat[1][0] + copy[1][0] * rhs.mat[1][1] + copy[2][0] * rhs.mat[1][2];
	mat[2][0] = copy[0][0] * rhs.mat[2][0] + copy[1][0] * rhs.mat[2][1] + copy[2][0] * rhs.mat[2][2];
	mat[0][1] = copy[0][1] * rhs.mat[0][0] + copy[1][1] * rhs.mat[0][1] + copy[2][1] * rhs.mat[0][2];
	mat[1][1] = copy[0][1] * rhs.mat[1][0] + copy[1][1] * rhs.mat[1][1] + copy[2][1] * rhs.mat[1][2];
	mat[2][1] = copy[0][1] * rhs.mat[2][0] + copy[1][1] * rhs.mat[2][1] + copy[2][1] * rhs.mat[2][2];
	mat[0][2] = copy[0][2] * rhs.mat[0][0] + copy[1][2] * rhs.mat[0][1] + copy[2][2] * rhs.mat[0][2];
	mat[1][2] = copy[0][2] * rhs.mat[1][0] + copy[1][2] * rhs.mat[1][1] + copy[2][2] * rhs.mat[1][2];
	mat[2][2] = copy[0][2] * rhs.mat[2][0] + copy[1][2] * rhs.mat[2][1] + copy[2][2] * rhs.mat[2][2];

	return *this;
}

Transform2D Transform2D::operator*(const Transform2D& rhs) const
{
	Transform2D result;

	result.mat[0][0] = mat[0][0] * rhs.mat[0][0] + mat[1][0] * rhs.mat[0][1] + mat[2][0] * rhs.mat[0][2];
	result.mat[1][0] = mat[0][0] * rhs.mat[1][0] + mat[1][0] * rhs.mat[1][1] + mat[2][0] * rhs.mat[1][2];
	result.mat[2][0] = mat[0][0] * rhs.mat[2][0] + mat[1][0] * rhs.mat[2][1] + mat[2][0] * rhs.mat[2][2];
	result.mat[0][1] = mat[0][1] * rhs.mat[0][0] + mat[1][1] * rhs.mat[0][1] + mat[2][1] * rhs.mat[0][2];
	result.mat[1][1] = mat[0][1] * rhs.mat[1][0] + mat[1][1] * rhs.mat[1][1] + mat[2][1] * rhs.mat[1][2];
	result.mat[2][1] = mat[0][1] * rhs.mat[2][0] + mat[1][1] * rhs.mat[2][1] + mat[2][1] * rhs.mat[2][2];
	result.mat[0][2] = mat[0][2] * rhs.mat[0][0] + mat[1][2] * rhs.mat[0][1] + mat[2][2] * rhs.mat[0][2];
	result.mat[1][2] = mat[0][2] * rhs.mat[1][0] + mat[1][2] * rhs.mat[1][1] + mat[2][2] * rhs.mat[1][2];
	result.mat[2][2] = mat[0][2] * rhs.mat[2][0] + mat[1][2] * rhs.mat[2][1] + mat[2][2] * rhs.mat[2][2];

	return result;
}

vec2 Transform2D::operator*(const vec2& rhs) const
{
	auto tmp = mat * vec4(rhs.x, rhs.y, 1.f, 1.f);
	return { tmp.x / tmp.z, tmp.y / tmp.z };
}

bool Transform2D::operator==(const Transform2D& rhs) const
{
	return mat == rhs.mat;
}

bool Transform2D::operator!=(const Transform2D& rhs) const
{
	return mat != rhs.mat;
}

VKDL_END