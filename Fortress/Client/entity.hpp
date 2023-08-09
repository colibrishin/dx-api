#pragma once
#ifndef BASEENTITY_HPP
#define BASEENTITY_HPP

#include <string>
#include <memory>

#include "virtual_this.hpp"

namespace Fortress::Abstract
{
	/**
	* \brief The basic information for entity, which includes base object, or scene, etc...
	*/
	class entity : public Dev::multi_inherit_this<entity>
	{
	public:
		entity() = delete;
		entity& operator=(const entity& other) = default;
		entity& operator=(entity&& other) = default;
		entity(const entity& other) = default;
		entity(entity&& other) = default;
		virtual ~entity() = default;

		__forceinline const std::wstring& get_name() const noexcept;

	protected:
		std::wstring m_name;

		explicit entity(const std::wstring name) : m_name(name)
		{
		}
	};

	__forceinline const std::wstring& entity::get_name() const noexcept
	{
		return m_name;
	}
}

#endif
