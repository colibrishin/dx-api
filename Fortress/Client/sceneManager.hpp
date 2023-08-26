#pragma once
#ifndef SCENEMANAGER_HPP
#define SCENEMANAGER_HPP

#include <map>
#include <string>
#include <memory>

#include "common.h"
#include "scene.hpp"

namespace Fortress
{
	namespace Abstract
	{
		class scene;
	}
}

namespace Fortress::Scene
{
	class BattleScene;

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
		static void CreateScene(Args... arg)
		{
			std::shared_ptr<T> scene = std::make_shared<T>(arg...);
			m_scenes[scene->get_name()] = scene;
		}	

		static void SetActive(const std::wstring& name);

		template <typename T>
		static void SetActive()
		{
			for(const auto& [name, scene] : m_scenes)
			{
				if(scene->downcast_from_this<T>())
				{
					m_current_scene.lock()->deactivate();
					m_current_scene = scene;
					m_current_scene.lock()->activate();
					return;
				}
			}
		}

		static std::weak_ptr<Abstract::scene> get_active_scene();
		static std::weak_ptr<BattleScene> get_active_map();

		static void remove_scene_by_name(const std::wstring& name);
		template <typename T>
		static void remove_scene()
		{
			Dev::erase_if(m_scenes, [](const auto& scene)
			{
				return std::dynamic_pointer_cast<T>(scene.second);
			});
		}
	private:
		// @todo: name can be different with scene
		inline static std::map<std::wstring, std::shared_ptr<Abstract::scene>> m_scenes = {};
		inline static std::weak_ptr<Abstract::scene> m_current_scene;
	};
}

#endif
