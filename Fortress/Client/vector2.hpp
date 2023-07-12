#ifndef Vector2_HPP
#define Vector2_HPP

#include <cmath>

namespace Fortress::Math
{
	/**
	 * \brief An error range for the float comparison.
	 */
	static constexpr float epsilon = 0.000001f;

	// High-level data structure for Vector2
	struct Vector2 final
	{
	public:
		Vector2() = default;
		Vector2(const Vector2& other) = default;
		Vector2(Vector2&& other) = default;
		~Vector2() = default;
		Vector2& operator=(Vector2&& other) = default;

		Vector2(const float x, const float y) : m_x(x), m_y(y)
		{
		}

		__forceinline Vector2& operator=(const Vector2& other) = default;
		__forceinline Vector2& operator+=(const Vector2& other);
		__forceinline Vector2& operator*=(const Vector2& other);
		__forceinline Vector2& operator-=(const Vector2& other);
		__forceinline Vector2 operator*(const Vector2& other) const;
		__forceinline Vector2 operator/(const float& scalar) const;
		__forceinline Vector2 operator*(const float& scalar) const;
		__forceinline Vector2 operator+(const Vector2& right) const;
		__forceinline Vector2 operator-() const;
		__forceinline Vector2 operator-(const Vector2& right) const;
		__forceinline bool operator==(const Vector2& other) const;
		__forceinline bool operator!=(const Vector2& other) const;


		__forceinline Vector2 top() const noexcept;
		__forceinline Vector2 bottom() const noexcept;
		__forceinline Vector2 left() const noexcept;
		__forceinline Vector2 right() const noexcept;

		__forceinline float magnitude() const;
		__forceinline float inner_product(const Vector2& other) const;
		__forceinline float inner_angle(const Vector2& other) const;
		__forceinline float dot_product(const Vector2& other) const;
		__forceinline float global_angle() const noexcept;
		__forceinline float get_x() const noexcept;
		__forceinline float get_y() const noexcept;
		__forceinline Vector2 reflect_x() const noexcept;
		__forceinline Vector2 reflect_y() const noexcept;
		__forceinline Vector2 abs() const noexcept;
		__forceinline Vector2 normalized() const noexcept;

	private:
		float m_x;
		float m_y;
	};
}

namespace Fortress::Math
{
	__forceinline Vector2& Vector2::operator+=(const Vector2& other)
	{
		m_x += other.m_x;
		m_y += other.m_y;
		return *this;
	}

	inline Vector2& Vector2::operator*=(const Vector2& other)
	{
		*this = *this * other;
		return *this;
	}

	__forceinline Vector2& Vector2::operator-=(const Vector2& other)
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
	__forceinline Vector2 Vector2::operator*(const Vector2& other) const
	{
		return {m_x * other.m_x, m_y * other.m_y};
	}

	__forceinline Vector2 Vector2::operator/(const float& scalar) const
	{
		return {m_x / scalar, m_y / scalar};
	}

	__forceinline Vector2 Vector2::operator*(const float& scalar) const
	{
		return {m_x * scalar, m_y * scalar};
	}

	__forceinline float Vector2::magnitude() const
	{
		return std::sqrtf(std::powf(m_x, 2) + std::powf(m_y, 2));
	}

	__forceinline float Vector2::inner_product(const Vector2& other) const
	{
		return dot_product(other) / (magnitude() * other.magnitude());
	}

	__forceinline float Vector2::inner_angle(const Vector2& other) const
	{
		return std::acosf(inner_product(other));
	}

	/**
	 * \brief A dot product of the two vectors.
	 * \param other a vector to get a product.
	 * \return A dot product scalar value.
	 */
	__forceinline float Vector2::dot_product(const Vector2& other) const
	{
		return m_x * other.m_x + m_y * other.m_y;
	}

	__forceinline Vector2 Vector2::operator+(const Vector2& right) const
	{
		return Vector2{m_x + right.m_x, m_y + right.m_y};
	}

	__forceinline Vector2 Vector2::operator-() const
	{
		return {-m_x, -m_y};
	}

	__forceinline Vector2 Vector2::operator-(const Vector2& right) const
	{
		return Vector2{m_x - right.m_x, m_y - right.m_y};
	}

	__forceinline bool Vector2::operator==(const Vector2& other) const
	{
		// rather safe comparison for float.
		return fabs(m_x - other.m_x) <= epsilon &&
			fabs(m_y - other.m_y) <= epsilon;
	}

	__forceinline bool Vector2::operator!=(const Vector2& other) const
	{
		// rather safe comparison for float.
		return !(*this == other);
	}

	__forceinline Vector2 Vector2::top() const noexcept
	{
		return Vector2{m_x + 0.0f, m_y + 1.0f};
	}

	__forceinline Vector2 Vector2::right() const noexcept
	{
		return Vector2{m_x + 1.0f, m_y + 0.0f};
	}

	__forceinline Vector2 Vector2::bottom() const noexcept
	{
		return Vector2{m_x + 0.0f, m_y - 1.0f};
	}

	__forceinline Vector2 Vector2::left() const noexcept
	{
		return Vector2{m_x - 1.0f, m_y + 0.0f};
	}

	__forceinline float Vector2::get_x() const noexcept
	{
		return m_x;
	}

	__forceinline float Vector2::get_y() const noexcept
	{
		return m_y;
	}

	/**
	 * \brief A reflection version of a given vector.
	 * \return A reflection vector that only applied in x-axis.
	 */
	__forceinline Vector2 Vector2::reflect_x() const noexcept
	{
		return *this + Vector2{m_x * -2.0f, 0};
	}

	/**
	 * \brief A reflection version of a given vector.
	 * \return A reflection vector that only applied in y-axis.
	 */
	__forceinline Vector2 Vector2::reflect_y() const noexcept
	{
		return *this + Vector2{0, m_y * -2.0f};
	}

	/**
	 * \brief An absolute value vector.
	 * \return An absolute vector.
	 */
	__forceinline Vector2 Vector2::abs() const noexcept
	{
		return {std::fabs(m_x), std::fabs(m_y)};
	}

	/**
	 * \brief Gets the normalized version of a given vector.
	 * \return A normalized vector.
	 */
	__forceinline Vector2 Vector2::normalized() const noexcept
	{
		return {m_x / magnitude(), m_y / magnitude()};
	}

	__forceinline float Vector2::global_angle() const noexcept
	{
		return atanf(m_y / m_x);
	}
}

#endif // Vector2_HPP
