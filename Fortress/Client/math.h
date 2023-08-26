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
	constexpr float G_ACC_DEFAULT = 9.80f;
	constexpr float G_ACC = G_ACC_DEFAULT * 1.5f;

	inline float flip_radian_polarity(const float radian)
	{
		if(radian < 0.0f)
		{
			return Math::PI + radian;
		}

		return Math::PI - radian;
	}

	inline float flip_radian(const float radian)
	{
		return radian - Math::PI;
	}

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
