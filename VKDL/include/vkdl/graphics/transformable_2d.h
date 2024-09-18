#pragma once

#include "../math/transform_2d.h"
#include "../util/property.hpp"

VKDL_BEGIN

class Transformable2D abstract
{
	PROPERTY_INIT(Transformable2D);

public:
	VKDL_INLINE Transformable2D();
	Transformable2D(const Transformable2D&) = default;
	Transformable2D(Transformable2D&&) VKDL_NOEXCEPT = default;

	Transformable2D& operator=(const Transformable2D&) = default;
	Transformable2D& operator=(Transformable2D&&) VKDL_NOEXCEPT = default;

	PROPERTY {
		PROPERTY_GET_SET(vec2,   Origin);
		PROPERTY_GET_SET(vec2,   Position);
		PROPERTY_GET_SET(vec2,   Scale);
		PROPERTY_GET_SET(radian, Rotation);
		PROPERTY_GET_SET(radian, ShearX);
		PROPERTY_GET_SET(radian, ShearY);
	};

	VKDL_INLINE void move(float delta_x, float delta_y);
	VKDL_INLINE void move(const vec2& delta);

	VKDL_INLINE void rotate(radian rad);

private:
	VKDL_INLINE PROPERTY_DECL_GET(Origin);
	VKDL_INLINE PROPERTY_DECL_SET(Origin);
	VKDL_INLINE PROPERTY_DECL_GET(Position);
	VKDL_INLINE PROPERTY_DECL_SET(Position);
	VKDL_INLINE PROPERTY_DECL_GET(Scale);
	VKDL_INLINE PROPERTY_DECL_SET(Scale);
	VKDL_INLINE PROPERTY_DECL_GET(Rotation);
	VKDL_INLINE PROPERTY_DECL_SET(Rotation);
	VKDL_INLINE PROPERTY_DECL_GET(ShearX);
	VKDL_INLINE PROPERTY_DECL_SET(ShearX);
	VKDL_INLINE PROPERTY_DECL_GET(ShearY);
	VKDL_INLINE PROPERTY_DECL_SET(ShearY);

protected:
	VKDL_INLINE Transform2D getTransform() const;

private:
	vec2   position;
	vec2   origin;
	vec2   scale;
	radian rotation;
	radian shear_x;
	radian shear_y;

	mutable Transform2D mat;
	mutable bool        updated;

};

Transformable2D::Transformable2D() :
	mat(),
	position(),
	origin(),
	scale(1.f, 1.f),
	rotation(0.f),
	shear_x(0.f),
	shear_y(0.f),
	updated(true)
{
}

VKDL_INLINE PROPERTY_IMPL_GET(Transformable2D, Origin)
{
	return origin;
}

VKDL_INLINE PROPERTY_IMPL_SET(Transformable2D, Origin)
{
	origin  = value;
	updated = false;
}

VKDL_INLINE PROPERTY_IMPL_GET(Transformable2D, Position)
{
	return position;
}

VKDL_INLINE PROPERTY_IMPL_SET(Transformable2D, Position)
{
	position = value;
	updated  = false;
}

VKDL_INLINE PROPERTY_IMPL_GET(Transformable2D, Scale)
{
	return scale;
}

VKDL_INLINE PROPERTY_IMPL_SET(Transformable2D, Scale)
{
	scale   = value;
	updated = false;
}

VKDL_INLINE PROPERTY_IMPL_GET(Transformable2D, Rotation)
{
	return rotation;
}

VKDL_INLINE PROPERTY_IMPL_SET(Transformable2D, Rotation)
{
	rotation = value;
	updated  = false;
}

VKDL_INLINE PROPERTY_IMPL_GET(Transformable2D, ShearX)
{
	return shear_x;
}

VKDL_INLINE PROPERTY_IMPL_SET(Transformable2D, ShearX)
{
	shear_x = value;
	updated = false;
}

VKDL_INLINE PROPERTY_IMPL_GET(Transformable2D, ShearY)
{
	return shear_y;
}

VKDL_INLINE PROPERTY_IMPL_SET(Transformable2D, ShearY)
{
	shear_y = value;
	updated = false;
}

VKDL_INLINE void Transformable2D::move(float delta_x, float delta_y)
{
	position.x += delta_x;
	position.y += delta_y;
	updated = false;
}

VKDL_INLINE void Transformable2D::move(const vec2& delta)
{
	position.x += delta.x;
	position.y += delta.y;
	updated = false;
}

VKDL_INLINE void Transformable2D::rotate(radian rad)
{
	rotation += rad;
	updated = false;
}

VKDL_INLINE Transform2D Transformable2D::getTransform() const
{
	if (!updated) {
		mat = Transform2D()
			.translate(position)
			.shear(shear_x, shear_y)
			.rotate(rotation)
			.scale(scale)
			.translate(origin);
		updated = true;
	}

	return mat;
}

VKDL_END