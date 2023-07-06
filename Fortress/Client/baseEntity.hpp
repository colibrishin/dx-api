#pragma once
#ifndef BASEENTITY_HPP
#define BASEENTITY_HPP

#include <string>

namespace Fortress 
{
	class _baseEntity
	{
	public:
		const std::wstring& getName() const noexcept
		{
			return m_name;
		}
		_baseEntity& operator=(const _baseEntity& other) 
		{
			m_name = other.m_name;
			return *this;
		}
	protected:
		std::wstring m_name;
		_baseEntity(const std::wstring& name) : m_name(name)
		{
		}
	};
}

#endif