#pragma once
#ifndef SCENEMANAGER_HPP
#define SCENEMANAGER_HPP

#include <map>
#include <string>
#include "scene.hpp"
#include <memory>
#include "titleScene.hpp"

namespace Fortress 
{
	class SceneManager
	{
	public:
		~SceneManager() 
		{
			for (auto& s : m_scenes) 
			{
				delete &s;
			}
		}
		static void initialize() 
		{
			auto title = CreateScene<TitleScene>(L"Title Scene");
			m_current_scene = title;
		}
		static void update();
		static void render();

		template <typename T>
		static T* CreateScene(const std::wstring& name)
		{
			T* scene = new T(name);
			m_scenes.emplace(&scene->getName(), scene);
			scene->initialize();
		}

		template <typename T>
		static void LoadScene(const T& scene)
		{
			m_scenes.emplace(&scene.getName(), &scene);
		}

		static void SetActive(const std::wstring& name)
		{
			auto scene = std::find(m_scenes.begin(), m_scenes.end(), [name](const std::wstring* ptr)
				{
					return *ptr == name;
				});

			if (scene != m_scenes.end()) 
			{
				m_current_scene = (*scene).second;
			}
		}
	private:
		inline static std::map<std::wstring*, _scene*> m_scenes = {};
		inline static _scene* m_current_scene;
	};
}

#endif