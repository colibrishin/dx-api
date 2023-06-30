#ifndef VECTOR2_HPP
#define VECTOR2_HPP

#include <cmath>

namespace Math
{
	static constexpr float epsilon = 0.0001f;

	// High-level data structure for vector2
	struct vector2 final
	{
	public:
		vector2(const float x, const float y) : m_x(x), m_y(y) {}

		virtual vector2& operator=(const vector2& other)
		{
			m_x = other.m_x;
			m_y = other.m_y;
			return *this;
		}

		virtual vector2& operator+=(const vector2& other)
		{
			m_x += other.m_x;
			m_y += other.m_y;
			return *this;
		}

		virtual vector2 operator+(const vector2& right) const
		{
			return vector2{ m_x + right.m_x, m_y + right.m_y };
		}
		virtual vector2 operator-(const vector2& right) const
		{
			return vector2{ m_x - right.m_x, m_y - right.m_y };
		}
		virtual bool operator==(const vector2& other) const
		{
			// rather safe comparison for float.
			return fabs(m_x - other.m_x) < Math::epsilon &&
				fabs(m_y - other.m_y) < Math::epsilon;
		}

		virtual ~vector2() = default;

		__forceinline vector2 top() const noexcept;
		__forceinline vector2 bottom() const noexcept;
		__forceinline vector2 left() const noexcept;
		__forceinline vector2 right() const noexcept;
		float global_angle() const noexcept;
		__forceinline float get_x() const noexcept;
		__forceinline float get_y() const noexcept;
	private:
		float m_x;
		float m_y;
	};

	vector2 vector2::top() const noexcept
	{
		return vector2{ m_x + 0.0f, m_y + 1.0f };
	}

	vector2 vector2::right() const noexcept
	{
		return vector2{ m_x + 1.0f, m_y + 0.0f };
	}

	vector2 vector2::bottom() const noexcept
	{
		return vector2{ m_x + 0.0f, m_y - 1.0f };
	}

	vector2 vector2::left() const noexcept
	{
		return vector2{ m_x - 1.0f, m_y + 0.0f };
	}

	float vector2::get_x() const noexcept
	{
		return m_x;
	}

	float vector2::get_y() const noexcept
	{
		return m_y;
	}
}

#endif // VECTOR2_HPP
