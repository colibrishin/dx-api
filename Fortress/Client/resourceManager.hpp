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
		~ResourceManager();
		template <typename T>
		static T* load(const std::wstring& name, const std::filesystem::path& path);
		template <typename T>
		static T* Find(const std::wstring& name) noexcept;
	private:
		inline static std::map<std::wstring, std::shared_ptr<Abstract::Resource>> m_resources = {};
	};

	inline ResourceManager::~ResourceManager()
	{
		for (auto& resource : m_resources)
		{
			resource.second.reset();
		}
	}

	template <typename T>
	T* ResourceManager::load(const std::wstring& name, const std::filesystem::path& path)
	{
		if(auto resource = Find<T>(name))
		{
			return resource;
		}

		m_resources[name] = std::make_shared<T>(name, path, Math::Vector2(0.0f, 0.0f));
		m_resources[name]->load();

		return dynamic_cast<T*>(m_resources[name].get());
	}

	template <typename T>
	T* ResourceManager::Find(const std::wstring& name) noexcept
	{
		if(m_resources.find(name) != m_resources.end())
		{
			return dynamic_cast<T*>(m_resources[name].get());
		}

		return nullptr;
	}
}
#endif // RESOURCEMANAGER_HPP
