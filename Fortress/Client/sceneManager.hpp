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

		~SceneManager()
		{
			for (auto& [_, ptr] : m_scenes)
			{
				ptr.reset();
			}
		}

		static void initialize();
		static void update();
		static void render();

		template <typename T>
		static T* CreateScene();
		static void SetActive(const std::wstring& name);
		static Abstract::scene* get_active_scene();

	private:
		// @todo: name can be different with scene
		inline static std::map<std::wstring, std::shared_ptr<Abstract::scene>> m_scenes = {};
		inline static std::shared_ptr<Abstract::scene> m_current_scene;
	};

	inline void SceneManager::initialize()
	{
	}

	inline void SceneManager::update()
	{
		m_current_scene->update();
	}

	inline void SceneManager::render()
	{
		m_current_scene->render();
	}

	inline void SceneManager::SetActive(const std::wstring& name)
	{
		const auto scene = m_scenes.find(name);

		if (scene != m_scenes.end())
		{
			if (m_current_scene)
			{
				m_current_scene->deactivate();
			}

			m_current_scene = (*scene).second;
			m_current_scene->activate();
		}
	}

	inline Abstract::scene* SceneManager::get_active_scene()
	{
		return m_current_scene.get();
	}

	template <typename T>
	T* SceneManager::CreateScene()
	{
		std::shared_ptr<T> scene = std::make_shared<T>();
		m_scenes.emplace(scene->get_name(), scene);
		scene->initialize();
		return scene.get();
	}

}

#endif
