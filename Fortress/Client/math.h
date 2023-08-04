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
	
	const Vector2 forward = {1.0f, 0.0f};
	const Vector2 up = {0.0f, -1.0f};
	const Vector2 vector_inf = {INFINITY, INFINITY};

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

	static Math::Vector2 to_hit_vector(const Math::Vector2& hit_left, const Math::Vector2& hit_right)
	{
		return (hit_left - hit_right).normalized();
	}
	
	static eHitVector translate_hit_vector(const Math::Vector2& hit_vector)
	{
		const auto abs_vector = hit_vector.abs();
		const auto orthogonal = abs_vector.get_x() > 1.0f - Math::epsilon || abs_vector.get_x() <= Math::epsilon;
		const auto parallel = abs_vector.get_y() > 1.0f - Math::epsilon || abs_vector.get_y() <= Math::epsilon;
		const auto left_check = hit_vector.get_x() < 0;
		const auto right_check = hit_vector.get_x() > 0;
		const auto top_check = hit_vector.get_y() < 0;
		const auto bottom_check = hit_vector.get_y() > 0;

		if(hit_vector == Math::Vector2{0.0f, 0.0f})
		{
			return eHitVector::Identical;
		}
		if(orthogonal && top_check)
		{
			return eHitVector::Top;
		}
		if(orthogonal && bottom_check)
		{
			return eHitVector::Bottom;
		}
		if(left_check && parallel)
		{
			return eHitVector::Left;
		}
		if(right_check && parallel)
		{
			return eHitVector::Right;
		}
		if(left_check && top_check)
		{
			return eHitVector::TopLeft;
		}
		if(right_check && top_check)
		{
			return eHitVector::TopRight;
		}
		if(left_check && bottom_check)
		{
			return eHitVector::BottomLeft;
		}
		if(right_check && bottom_check)
		{
			return eHitVector::BottomRight;
		}

		return eHitVector::Unknown;
	}
}

#endif
