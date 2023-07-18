#pragma once

#ifndef MATH_H
#define MATH_H

#define _USE_MATH_DEFINES

#include <math.h>

namespace Fortress::Math
{
	/**
	 * \brief Pi
	 */
	constexpr float PI = M_PI;

	constexpr float E = M_E;

	/**
	 * \brief Gravity constant
	 */
	constexpr float G = 6.6742E-11;

	/**
	 * \brief Gravitational acceleration constant
	 */
	constexpr float G_ACC = 9.80f;

	const Vector2 identity = {1.0f, 1.0f};
	const Vector2 left = {-1.0f, 0.0f};
	const Vector2 right = {1.0f, 0.0f};
	const Vector2 top = {0.0f, -1.0f};
	const Vector2 bottom = {0.0f, 1.0f};

	inline float to_radian(const float degree)
	{
		return degree * (PI / 180);
	}

	inline float to_degree(const float radian)
	{
		return radian * (180 / PI);
	}

	inline float sigmod(const float t)
	{
		return 1 / (1 + std::powf(E, -t));
	}
}

#endif
