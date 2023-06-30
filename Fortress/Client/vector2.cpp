#include "vector2.hpp"

namespace Math
{
	float vector2::global_angle() const noexcept
	{
		return atanf(m_y / m_x);
	}
}

