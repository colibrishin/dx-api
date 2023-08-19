#include "pch.h"
#include "sceneManager.hpp"
#include "../Common/BattleScene.h"
#include "scene.hpp"

namespace Fortress::Scene
{
	void SceneManager::cleanup()
	{
		for(auto& scene : m_scenes)
		{
			scene.second.reset();
		}
	}

	void SceneManager::initialize()
	{
	}

	void SceneManager::update()
	{
		if(const std::shared_ptr<Abstract::scene> ptr = m_current_scene.lock())
		{
			ptr->update();
		}
	}

	void SceneManager::render()
	{
		if(const std::shared_ptr<Abstract::scene> ptr = m_current_scene.lock())
		{
			ptr->render();
		}
	}

	void SceneManager::SetActive(const std::wstring& name)
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

	std::weak_ptr<Abstract::scene> SceneManager::get_active_scene()
	{
		return m_current_scene;
	}

	std::weak_ptr<BattleScene> SceneManager::get_active_map()
	{
		return m_current_scene.lock()->downcast_from_this<BattleScene>();
	}

	void SceneManager::remove_scene_by_name(const std::wstring& name)
	{
		m_scenes.erase(name);
	}
}
