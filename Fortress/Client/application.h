#ifndef APPLICATION_H
#define APPLICATION_H
#pragma once

#include "common.h"
#include "resourceManager.hpp"
#include "sceneManager.hpp"
#include "winapihandles.hpp"

namespace Fortress
{
	class Application
	{
	public:
		Application() :
			m_hwnd(nullptr),
			m_hdc(nullptr),
			m_buffer_hdc(nullptr)
		{
		}

		~Application()
		{
			Resource::ResourceManager::cleanup();
		}
		Application& operator=(const Application&) = delete;

		void initialize(HWND, HDC);
		void update();
		void render();

	private:
		Resource::ResourceManager m_resource_manager;
		Scene::SceneManager m_scene_manager;
		WinAPIHandles m_win_handles;

		HWND m_hwnd;
		HDC m_hdc;
		HDC m_buffer_hdc;
	};
}

#endif
