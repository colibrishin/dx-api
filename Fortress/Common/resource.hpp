#pragma once
#ifndef RESOURCE_HPP
#define RESOURCE_HPP
#include <filesystem>

#include "entity.hpp"

namespace Fortress::Abstract
{
	class Resource : public entity
	{
	public:
		Resource() = delete;
		~Resource() override = default;
		Resource& operator=(const Resource& other) = default;
		Resource& operator=(Resource&& other) = default;

		virtual bool load() = 0;

		const std::filesystem::path& get_path() const;
		void set_path(const std::filesystem::path& path);
	protected:
		Resource(const std::wstring& name, const std::filesystem::path& path);
		Resource(const Resource& other) = default;
		Resource(Resource&& other) = default;
	private:
		std::filesystem::path m_path;
	};

	inline Resource::Resource(const std::wstring& name, const std::filesystem::path& path):
		entity(name),
		m_path(path)
	{
	}

	inline const std::filesystem::path& Resource::get_path() const
	{
		return m_path;
	}
}
#endif // RESOURCE_HPP
