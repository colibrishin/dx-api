#ifndef Vector2_HPP
#define Vector2_HPP

#include <cmath>

#include "common.h"

namespace Gdiplus
{
	class SizeF;
	class PointF;
}

namespace Fortress::Math
{
	/**
	 * \brief An error range for the float comparison.
	 */
	static constexpr float epsilon = 0.001f;

	// High-level data structure for Vector2
	struct Vector2 final
	{
	public:
		Vector2() = default;
		Vector2(const Vector2& other) = default;
		Vector2(Vector2&& other) = default;
		~Vector2() = default;
		Vector2& operator=(Vector2&& other) = default;

		operator Gdiplus::PointF () const;
		operator Gdiplus::SizeF () const;

		Vector2(const float x, const float y) : m_x(x), m_y(y)
		{
		}

		Vector2(const int x, const int y) : m_x(x), m_y(y)
		{
		}

		Vector2(const float x, const int y) : m_x(x), m_y(y)
		{
		}

		Vector2(const int x, const float y) : m_x(x), m_y(y)
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
		__forceinline Vector2 operator+(const float scalar) const;
		__forceinline Vector2 operator-() const;
		__forceinline Vector2 operator-(const Vector2& right) const;
		__forceinline Vector2 operator-(const float scalar) const;
		__forceinline bool operator==(const Vector2& other) const;
		__forceinline bool operator!=(const Vector2& other) const;


		__forceinline Vector2 top() const noexcept;
		__forceinline Vector2 bottom() const noexcept;
		__forceinline Vector2 left() const noexcept;
		__forceinline Vector2 right() const noexcept;

		__forceinline float magnitude() const;
		__forceinline float inner_product(const Vector2& other) const;
		__forceinline float global_inner_angle(const Vector2& other) const;
		__forceinline float local_inner_angle(const Vector2& other) const;
		__forceinline float dot_product(const Vector2& other) const;
		__forceinline float global_angle() const noexcept;
		__forceinline float get_x() const noexcept;
		__forceinline Vector2 rotate(float radian) const noexcept;
		__forceinline float get_y() const noexcept;
		__forceinline Vector2 reflect_x() const noexcept;
		__forceinline Vector2 reflect_y() const noexcept;
		__forceinline Vector2 abs() const noexcept;
		__forceinline Vector2 normalized() const noexcept;
		__forceinline float unit_angle() const noexcept;
		__forceinline static UnitVector angle_vector(const float) noexcept;
		__forceinline Vector2 x_dir() const noexcept;
		__forceinline static Vector2 to_dir_vector(const Math::Vector2& hit_left, const Math::Vector2& hit_right);
		__forceinline static eDirVector to_dir_enum(const Math::Vector2& hit_vector);

	private:
		float m_x;
		float m_y;
	};

	
	const Vector2 identity = {1.0f, 1.0f};
	const Vector2 left = {-1.0f, 0.0f};
	const Vector2 right = {1.0f, 0.0f};
	const Vector2 top = {0.0f, -1.0f};
	const Vector2 bottom = {0.0f, 1.0f};
	const Vector2 zero = {};
	
	const Vector2 forward = {1.0f, 0.0f};
	const Vector2 up = {0.0f, -1.0f};
	const Vector2 vector_inf = {INFINITY, INFINITY};
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

	__forceinline float Vector2::global_inner_angle(const Vector2& other) const
	{
		return std::acosf(inner_product(other));
	}

	inline float Vector2::local_inner_angle(const Vector2& other) const
	{
		return std::atanf((m_y - other.m_y) / (m_x - other.m_x));
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

	inline Vector2 Vector2::operator+(const float scalar) const
	{
		return Vector2{m_x + scalar, m_y + scalar};
	}

	__forceinline Vector2 Vector2::operator-() const
	{
		return {-m_x, -m_y};
	}

	__forceinline Vector2 Vector2::operator-(const Vector2& right) const
	{
		return Vector2{m_x - right.m_x, m_y - right.m_y};
	}

	inline Vector2 Vector2::operator-(const float scalar) const
	{
		return Vector2{m_x - scalar, m_y - scalar};
	}

	__forceinline bool Vector2::operator==(const Vector2& other) const
	{
		if(m_x == INFINITY &&
			m_y == INFINITY &&
			other.m_x == INFINITY && 
			other.m_y == INFINITY)
		{
			return true;
		}

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

	__forceinline Vector2 Vector2::rotate(const float radian) const noexcept
	{
		return
		{m_x * cosf(radian) - m_y * sinf(radian),
			m_x * sinf(radian) + m_y * cosf(radian)};
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
		return {m_x * -2.0f, m_y};
	}

	/**
	 * \brief A reflection version of a given vector.
	 * \return A reflection vector that only applied in y-axis.
	 */
	__forceinline Vector2 Vector2::reflect_y() const noexcept
	{
		return Vector2{m_x, m_y * -2.0f};
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

	inline float Vector2::unit_angle() const noexcept
	{
		return std::atan2(m_y, m_x);
	}

	inline UnitVector Vector2::angle_vector(const float angle) noexcept
	{
		return {cosf(angle), sinf(angle)};
	}

	__forceinline float Vector2::global_angle() const noexcept
	{
		return atanf(m_y / m_x);
	}

	__forceinline DirVector Vector2::x_dir() const noexcept
	{
		return m_x < 0 ? Math::left : Math::right;
	}

	__forceinline DirVector Vector2::to_dir_vector(const Math::Vector2& hit_left, const Math::Vector2& hit_right)
	{
		return (hit_left - hit_right).normalized();
	}

	__forceinline eDirVector Vector2::to_dir_enum(const DirVector& hit_vector)
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
			return eDirVector::Identical;
		}
		if(orthogonal && top_check)
		{
			return eDirVector::Top;
		}
		if(orthogonal && bottom_check)
		{
			return eDirVector::Bottom;
		}
		if(left_check && parallel)
		{
			return eDirVector::Left;
		}
		if(right_check && parallel)
		{
			return eDirVector::Right;
		}
		if(left_check && top_check)
		{
			return eDirVector::TopLeft;
		}
		if(right_check && top_check)
		{
			return eDirVector::TopRight;
		}
		if(left_check && bottom_check)
		{
			return eDirVector::BottomLeft;
		}
		if(right_check && bottom_check)
		{
			return eDirVector::BottomRight;
		}

		return eDirVector::Unknown;
	}
}

#endif // Vector2_HPP
