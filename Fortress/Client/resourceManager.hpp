#pragma once
#ifndef RESOURCEMANAGER_HPP
#define RESOURCEMANAGER_HPP
#include <map>
#include <string>

#include "vector2.hpp"
#include "resource.hpp"

namespace Fortress::Resource
{
	class ResourceManager
	{
	public:
		ResourceManager() = default;
		~ResourceManager() = default;
		template <typename T>
		static std::shared_ptr<T> load(const std::wstring& name, const std::filesystem::path& path);
		template <typename T>
		static std::shared_ptr<T> Find(const std::wstring& name) noexcept;
	private:
		inline static std::map<std::wstring, std::shared_ptr<Abstract::Resource>> m_resources = {};
	};

	template <typename T>
	std::shared_ptr<T> ResourceManager::load(const std::wstring& name, const std::filesystem::path& path)
	{
		if(auto resource = Find<T>(name))
		{
			return resource;
		}

		m_resources[name] = std::make_shared<T>(name, path, Math::Vector2(0.0f, 0.0f));
		m_resources[name]->load();

		return std::dynamic_pointer_cast<T>(m_resources[name]);
	}

	template <typename T>
	std::shared_ptr<T> ResourceManager::Find(const std::wstring& name) noexcept
	{
		if(m_resources.find(name) != m_resources.end())
		{
			return std::dynamic_pointer_cast<T>(m_resources[name]);
		}

		return nullptr;
	}
}
#endif // RESOURCEMANAGER_HPP
