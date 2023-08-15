#pragma once
#ifndef RESOURCEMANAGER_HPP
#define RESOURCEMANAGER_HPP
#include <cassert>
#include <map>
#include <string>

#include "GifWrapper.h"
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
		static std::weak_ptr<T> load(const std::wstring& name, const std::filesystem::path& path);
		template <typename T>
		static void unload(const std::wstring& name);
		template <typename T>
		static std::weak_ptr<T> find(const std::wstring& name) noexcept;
		static void cleanup();

	private:
		template <typename T>
		static std::shared_ptr<T> find_internally(const std::wstring& name) noexcept;
		inline static std::map<std::wstring, std::shared_ptr<Abstract::Resource>> m_resources = {};
	};

	inline void ResourceManager::cleanup()
	{
		for(auto& [_, p] : m_resources)
		{
			p.reset();
		}
	}

	template <typename T>
	std::weak_ptr<T> ResourceManager::load(const std::wstring& name, const std::filesystem::path& path)
	{
		if(auto resource = find_internally<T>(name))
		{
			return resource;
		}

		const auto created = std::make_shared<T>(name, path);
		m_resources[name] = created;
		return created;
	}

	template <typename T>
	void ResourceManager::unload(const std::wstring& name)
	{
		static_assert(
			std::is_same_v<std::shared_ptr<T>, 
			decltype(std::dynamic_pointer_cast<T>(m_resources[name]))>);

		m_resources[name].reset();
		m_resources.erase(name);
	}

	template<typename T>
	inline std::weak_ptr<T> ResourceManager::find(const std::wstring& name) noexcept
	{
		if(m_resources.find(name) != m_resources.end())
		{
			return std::dynamic_pointer_cast<T>(m_resources[name]);
		}

		return std::weak_ptr<T>();
	}

	template <typename T>
	std::shared_ptr<T> ResourceManager::find_internally(const std::wstring& name) noexcept
	{
		if(m_resources.find(name) != m_resources.end())
		{
			return std::dynamic_pointer_cast<T>(m_resources[name]);
		}

		return nullptr;
	}
}
#endif // RESOURCEMANAGER_HPP
