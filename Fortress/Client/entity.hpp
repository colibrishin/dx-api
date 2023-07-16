#pragma once
#ifndef BASEENTITY_HPP
#define BASEENTITY_HPP

#include <string>
#include <memory>

namespace Fortress::Abstract
{
	/**
	* \brief The basic information for entity, which includes base object, or scene, etc...
	*/
	class entity : public std::enable_shared_from_this<entity>
	{
	public:
		entity() = delete;
		entity& operator=(const entity& other) = default;
		entity& operator=(entity&& other) = default;
		virtual ~entity() = default;

		__forceinline const std::wstring& get_name() const noexcept;

	protected:
		std::wstring m_name;

		explicit entity(std::wstring name) : m_name(std::move(name))
		{
		}

		entity(const entity& other) = default;

		entity(entity&& other) noexcept
		{
			m_name = std::move(other.m_name);
		}
	};

	__forceinline const std::wstring& entity::get_name() const noexcept
	{
		return m_name;
	}
}

#endif
