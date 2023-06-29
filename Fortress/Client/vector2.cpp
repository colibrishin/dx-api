#include "vector2.hpp"

namespace MathInternal
{
	vector2 vector2::top() const noexcept
	{
		return vector2{ this->m_x + 0.0f, this->m_y + 1.0f };
	}

	vector2 vector2::right() const noexcept
	{
		return vector2{ this->m_x + 1.0f, this->m_y + 0.0f };
	}

	vector2 vector2::bottom() const noexcept 
	{
		return vector2{ this->m_x + 0.0f, this->m_y - 1.0f };
	}

	vector2 vector2::left() const noexcept 
	{
		return vector2{ this->m_x - 1.0f, this->m_y + 0.0f };
	}

	float vector2::global_angle() const noexcept
	{
		return atanf(this->m_y / this->m_x);
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

