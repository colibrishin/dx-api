#ifndef VECTOR2_HPP
#define VECTOR2_HPP

#include <cmath>

#include "math.h"

namespace Math
{
	/**
	 * \brief An error range for the float comparison.
	 */
	static constexpr float epsilon = 0.0001f;

	// High-level data structure for vector2
	struct vector2 final
	{
	public:
		vector2() = default;

		vector2(const float x, const float y) : m_x(x), m_y(y)
		{
		}

		vector2& operator=(const vector2& other) = default;

		vector2& operator+=(const vector2& other)
		{
			m_x += other.m_x;
			m_y += other.m_y;
			return *this;
		}

		vector2& operator-=(const vector2& other)
		{
			m_x -= other.m_x;
			m_y -= other.m_y;
			return *this;
		}

		/**
		 * \brief A simple multiplication of the two vectors.
		 * \param other another vector to multiply.
		 * \return {A.x * B.x, A.y * B.y}
		 */
		vector2 operator*(const vector2& other) const
		{
			return {m_x * other.m_x, m_y * other.m_y};
		}

		vector2 operator/(const float& scalar) const
		{
			return {m_x / scalar, m_y / scalar};
		}

		vector2 operator*(const float& scalar) const
		{
			return {m_x * scalar, m_y * scalar};
		}

		float magnitude() const
		{
			return std::sqrtf(std::powf(m_x, 2) + std::powf(m_y, 2));
		}

		float inner_product(const vector2& other) const
		{
			return dot_product(other) / (magnitude() * other.magnitude());
		}

		float inner_angle(const vector2& other) const
		{
			return std::acosf(inner_product(other));
		}

		/**
		 * \brief A dot product of the two vectors.
		 * \param other a vector to get a product.
		 * \return A dot product scalar value.
		 */
		float dot_product(const vector2& other) const
		{
			return m_x * other.m_x + m_y * other.m_y;
		}

		vector2 operator+(const vector2& right) const
		{
			return vector2{m_x + right.m_x, m_y + right.m_y};
		}

		vector2 operator-() const
		{
			return {-m_x, -m_y};
		}

		vector2 operator-(const vector2& right) const
		{
			return vector2{m_x - right.m_x, m_y - right.m_y};
		}

		bool operator==(const vector2& other) const
		{
			// rather safe comparison for float.
			return fabs(m_x - other.m_x) <= epsilon &&
				fabs(m_y - other.m_y) <= epsilon;
		}

		bool operator!=(const vector2& other) const
		{
			// rather safe comparison for float.
			return fabs(m_x - other.m_x) > epsilon &&
				fabs(m_y - other.m_y) > epsilon;
		}

		virtual ~vector2() = default;

		__forceinline vector2 top() const noexcept;
		__forceinline vector2 bottom() const noexcept;
		__forceinline vector2 left() const noexcept;
		__forceinline vector2 right() const noexcept;
		float global_angle() const noexcept;
		__forceinline float get_x() const noexcept;
		__forceinline float get_y() const noexcept;
		__forceinline vector2 reflect_x() const noexcept;
		__forceinline vector2 reflect_y() const noexcept;
		__forceinline vector2 abs() const noexcept;
		__forceinline vector2 normalized() const noexcept;

	private:
		float m_x;
		float m_y;
	};

	vector2 vector2::top() const noexcept
	{
		return vector2{m_x + 0.0f, m_y + 1.0f};
	}

	vector2 vector2::right() const noexcept
	{
		return vector2{m_x + 1.0f, m_y + 0.0f};
	}

	vector2 vector2::bottom() const noexcept
	{
		return vector2{m_x + 0.0f, m_y - 1.0f};
	}

	vector2 vector2::left() const noexcept
	{
		return vector2{m_x - 1.0f, m_y + 0.0f};
	}

	float vector2::get_x() const noexcept
	{
		return m_x;
	}

	float vector2::get_y() const noexcept
	{
		return m_y;
	}

	/**
	 * \brief A reflection version of a given vector.
	 * \return A reflection vector that only applied in x-axis.
	 */
	__forceinline vector2 vector2::reflect_x() const noexcept
	{
		return *this + vector2{m_x * -2.0f, 0};
	}

	/**
	 * \brief A reflection version of a given vector.
	 * \return A reflection vector that only applied in y-axis.
	 */
	__forceinline vector2 vector2::reflect_y() const noexcept
	{
		return *this + vector2{0, m_y * -2.0f};
	}

	/**
	 * \brief An absolute value vector.
	 * \return An absolute vector.
	 */
	__forceinline vector2 vector2::abs() const noexcept
	{
		return {std::fabs(m_x), std::fabs(m_y)};
	}

	/**
	 * \brief Gets the normalized version of a given vector.
	 * \return A normalized vector.
	 */
	__forceinline vector2 vector2::normalized() const noexcept
	{
		return {m_x / magnitude(), m_y / magnitude()};
	}
}

#endif // VECTOR2_HPP
