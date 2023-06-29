#ifndef VECTOR2_HPP
#define VECTOR2_HPP

#include <cmath>
#include "math.h"

namespace MathInternal
{
	struct _vector2
	{
	public:
		_vector2() : m_x(0.0f), m_y(0.0f) {}

		_vector2(const float x, const float y) : m_x(x), m_y(y) {}

		virtual ~_vector2() = default;

		_vector2& operator=(const _vector2& other) = default;

		_vector2 operator+(const _vector2& right) const
		{
			return _vector2{ m_x + right.m_x, m_y + right.m_y };
		}
		_vector2 operator-(const _vector2& right) const
		{
			return _vector2{ m_x - right.m_x, m_y - right.m_y };
		}
		bool operator==(const _vector2& other) const
		{
			// rather safe comparison for float.
			return fabs(m_x - other.m_x) < epsilon && 
				fabs(m_y - other.m_y) < epsilon;
		}

	protected:
		float m_x;
		float m_y;
	};

	// High-level data structure for vector2
	struct vector2 final : _vector2
	{
	public:
		vector2(const float x, const float y) : _vector2(x, y) {}

		virtual ~vector2() = default;
		vector2& operator =(const vector2& other) = default;

		vector2 top() const noexcept;
		vector2 bottom() const noexcept;
		vector2 left() const noexcept;
		vector2 right() const noexcept;
		float global_angle() const noexcept;
		float get_x() const noexcept;
		float get_y() const noexcept;
	};
}

#endif // VECTOR2_HPP
