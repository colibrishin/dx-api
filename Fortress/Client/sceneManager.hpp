#pragma once
#ifndef SCENEMANAGER_HPP
#define SCENEMANAGER_HPP

#include <map>
#include <string>
#include "framework.h"
#include "baseEntity.hpp"

namespace Scene 
{
	class SceneManager final
	{
	public:
		class _scene : public Fortress::_baseEntity
		{
		public:
			_scene(const std::wstring& name) : _baseEntity(name), m_hwnd(SceneManager::m_hwnd), m_hdc(SceneManager::m_hdc)
			{
			}

			virtual void initialize() {}
			virtual void update() {}
			virtual void render() {}
		protected:
			HWND m_hwnd = nullptr;
			HDC m_hdc = nullptr;
		};

		SceneManager() = default;
		~SceneManager() 
		{
			for (const auto& [_, ptr] : m_scenes) 
			{
				delete ptr;
			}
		}
		static void initialize(HWND hwnd, HDC hdc) 
		{
			m_hwnd = hwnd;
			m_hdc = hdc;
		}
		static void update()
		{
			m_current_scene->update();
		}
		static void render()
		{
			m_current_scene->render();
		}

		template <typename T>
		static T* CreateScene()
		{
			T* scene = new T();
			m_scenes.emplace(scene->getName(), scene);
			scene->initialize();
			return scene;
		}

		static void SetActive(const std::wstring& name)
		{
			auto scene = m_scenes.find(name);

			if (scene != m_scenes.end()) 
			{
				m_current_scene = (*scene).second;
			}
		}
	private:
		friend class _scene;
		// @todo: name can be different with scene
		inline static HWND m_hwnd = nullptr;
		inline static HDC m_hdc = nullptr;
		inline static std::map<std::wstring, _scene*> m_scenes = {};
		inline static _scene* m_current_scene;
	};
}

#endif