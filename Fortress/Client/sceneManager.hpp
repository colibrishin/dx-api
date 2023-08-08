#pragma once
#ifndef SCENEMANAGER_HPP
#define SCENEMANAGER_HPP

#include <map>
#include <string>
#include <memory>
#include "scene.hpp"

namespace Fortress::Scene
{
	class SceneManager final
	{
	public:
		SceneManager() = default;
		~SceneManager() = default;
		static void cleanup();

		static void initialize();
		static void update();
		static void render();

		template <typename T, typename... Args>
		static std::weak_ptr<T> CreateScene(Args... args);

		static void SetActive(const std::wstring& name);
		static std::weak_ptr<Abstract::scene> get_active_scene();

		static void remove_scene_by_name(const std::wstring& name);
		template <class T>
		static void remove_scene();
	private:
		// @todo: name can be different with scene
		inline static std::map<std::wstring, std::shared_ptr<Abstract::scene>> m_scenes = {};
		inline static std::weak_ptr<Abstract::scene> m_current_scene;
	};

	inline void SceneManager::cleanup()
	{
		for(auto& scene : m_scenes)
		{
			scene.second.reset();
		}
	}

	inline void SceneManager::initialize()
	{
	}

	inline void SceneManager::update()
	{
		if(const std::shared_ptr<Abstract::scene> ptr = m_current_scene.lock())
		{
			ptr->update();
		}
	}

	inline void SceneManager::render()
	{
		if(const std::shared_ptr<Abstract::scene> ptr = m_current_scene.lock())
		{
			ptr->render();
		}
	}

	inline void SceneManager::SetActive(const std::wstring& name)
	{
		const auto scene = m_scenes.find(name);

		if (scene != m_scenes.end())
		{
			if (const auto& current_scene_ptr = m_current_scene.lock())
			{
				current_scene_ptr->deactivate();
			}
			
			m_current_scene = scene->second;
			m_current_scene.lock()->activate();
		}
	}

	inline std::weak_ptr<Abstract::scene> SceneManager::get_active_scene()
	{
		return m_current_scene;
	}

	inline void SceneManager::remove_scene_by_name(const std::wstring& name)
	{
		m_scenes.erase(name);
	}

	template <typename T, typename... Args>
	std::weak_ptr<T> SceneManager::CreateScene(Args... arg)
	{
		std::shared_ptr<T> scene = std::make_shared<T>(arg...);
		m_scenes.emplace(scene->get_name(), scene);
		scene->initialize();
		return scene;
	}

	template <typename T>
	void SceneManager::remove_scene()
	{
		erase_if(m_scenes, [](const auto& scene)
		{
			return std::dynamic_pointer_cast<T>(scene.second);
		});
	}
}

#endif
