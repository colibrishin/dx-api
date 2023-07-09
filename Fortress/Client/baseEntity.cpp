#include "baseEntity.hpp"

#include <string>

namespace Fortress
{
	__forceinline const std::wstring& _baseEntity::get_name() const noexcept
	{
		return m_name;
	}
}
