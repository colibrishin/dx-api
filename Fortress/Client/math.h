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

	static Math::Vector2 to_hit_vector(const Math::Vector2& hit_left, const Math::Vector2& hit_right)
	{
		return (hit_left - hit_right).normalized();
	}
	
	static eHitVector translate_hit_vector(const Math::Vector2& hit_vector)
	{
		const auto abs_vector = hit_vector.abs();

		if(hit_vector == Math::Vector2{0.0f, 0.0f})
		{
			return eHitVector::Identical;
		}
		if(abs_vector.get_x() < Math::epsilon && hit_vector.get_y() > 0)
		{
			return eHitVector::Top;
		}
		if(abs_vector.get_x() < Math::epsilon && hit_vector.get_y() < 0)
		{
			return eHitVector::Bottom;
		}
		if(hit_vector.get_x() > 0 && abs_vector.get_y() < Math::epsilon)
		{
			return eHitVector::Left;
		}
		if(hit_vector.get_x() < 0 && abs_vector.get_y() < Math::epsilon)
		{
			return eHitVector::Right;
		}
		if(hit_vector.get_x() > 0 && hit_vector.get_y() > 0)
		{
			return eHitVector::TopLeft;
		}
		if(hit_vector.get_x() < 0 && hit_vector.get_y() > 0)
		{
			return eHitVector::TopRight;
		}
		if(hit_vector.get_x() > 0 && hit_vector.get_y() < 0)
		{
			return eHitVector::BottomLeft;
		}
		if(hit_vector.get_x() < 0 && hit_vector.get_y() < 0)
		{
			return eHitVector::BottomRight;
		}

		return eHitVector::Unknown;
	}
}

#endif
