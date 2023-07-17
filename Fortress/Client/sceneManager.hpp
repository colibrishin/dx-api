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

		static void initialize();
		static void update();
		static void render();

		template <typename T>
		static std::weak_ptr<T> CreateScene();
		static void SetActive(const std::wstring& name);
		static std::weak_ptr<Abstract::scene> get_active_scene();

	private:
		// @todo: name can be different with scene
		inline static std::map<std::wstring, std::shared_ptr<Abstract::scene>> m_scenes = {};
		inline static std::weak_ptr<Abstract::scene> m_current_scene;
	};

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

	template <typename T>
	std::weak_ptr<T> SceneManager::CreateScene()
	{
		std::shared_ptr<T> scene = std::make_shared<T>();
		m_scenes.emplace(scene->get_name(), scene);
		scene->initialize();
		return scene;
	}

}

#endif
