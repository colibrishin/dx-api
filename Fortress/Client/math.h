#pragma once

#include "framework.h"
#include <cmath>

#ifndef MATH_H
#define MATH_H

namespace Math 
{
	// Low-level data struct for Vector2
	struct _Vector2
	{
	public:
		_Vector2() : m_x(0.0f), m_y(0.0f)
		{
		}

		_Vector2(const float x, const float y) : m_x(x), m_y(y)
		{
		}

		virtual ~_Vector2() = default;

		_Vector2 operator +(const _Vector2& right)
		{
			return _Vector2{ this->m_x + right.m_x, this->m_y + right.m_y };
		}
		_Vector2 operator -(const _Vector2& right) 
		{
			return _Vector2{ this->m_x - right.m_x, this->m_y - right.m_y };
		}
		bool operator==(const _Vector2& other) const
		{
			return this->m_x == other.m_x && this->m_y == other.m_y;
		}

	protected:
		float m_x;
		float m_y;
	};

	struct Vector2;

	// High-level data structure for Vector2
	struct Vector2 : _Vector2
	{
	public:
		Vector2(const float x, const float y) : _Vector2(x, y) {}

		virtual ~Vector2() = default;

		inline Vector2 top() const noexcept
		{
			return Vector2{ this->m_x + 0.0f, this->m_y + 1.0f };
		}

		inline Vector2 right() const noexcept
		{
			return Vector2{ this->m_x + 1.0f, this->m_y + 0.0f };
		}

		inline Vector2 bottom() const noexcept 
		{
			return Vector2{ this->m_x + 0.0f, this->m_y - 1.0f };
		}

		inline Vector2 left() const noexcept 
		{
			return Vector2{ this->m_x - 1.0f, this->m_y + 0.0f };
		}

		inline float global_angle() const noexcept
		{
			return atanf(this->m_y / this->m_x);
		}

		float get_x() const noexcept 
		{
			return m_x;
		}

		float get_y() const noexcept
		{
			return m_y;
		}

		void initialize();
		void run() const;
		void update();
		void render() const;
	};
}

#endif