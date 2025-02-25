#pragma once

#define M_PI 3.14159265358979323846

typedef float radian;

constexpr radian to_radian(float degree)
{
	return static_cast<radian>(degree * M_PI / 180.f);
}

constexpr radian operator"" _deg(long double degree)
{
	return static_cast<float>(degree * M_PI / 180.f);
}

#undef M_PI