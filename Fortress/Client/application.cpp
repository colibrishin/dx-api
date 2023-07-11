#include <random>

#include "application.h"
#include "characterScene.h"
#include "input.hpp"
#include "deltatime.hpp"
#include "sceneManager.hpp"
#include "titleScene.h"
#include "winapihandles.hpp"
#include "projectile.hpp"

namespace Fortress
{
	void Application::initialize(const HWND hwnd, const HDC hdc)
	{
		m_hwnd = hwnd;
		m_hdc = hdc;
		Input::initialize();
		DeltaTime::initialize();

		WinAPIHandles::initialize(hwnd, hdc);
		m_buffer_hdc = WinAPIHandles::get_buffer_dc();
		Debug::initialize(m_buffer_hdc);

		Scene::SceneManager::initialize();
		Scene::SceneManager::CreateScene<Scene::TitleScene>();
		Scene::SceneManager::CreateScene<Scene::CharacterScene>();
		Scene::SceneManager::SetActive(L"Title Scene");
	}

	void Application::update()
	{
		if (!m_hdc || !m_hwnd || !m_buffer_hdc)
		{
			return;
		}

		DeltaTime::update();
		Input::update();
		Scene::SceneManager::update();
	}

	void Application::render()
	{
		const auto hbrBkGnd = CreateSolidBrush(GetSysColor(COLOR_WINDOW));
		FillRect(m_buffer_hdc, &WinAPIHandles::get_window_size(), hbrBkGnd);
		DeleteObject(hbrBkGnd);

		DeltaTime::render();
		Debug::render();
		Scene::SceneManager::render();

		BitBlt(m_hdc, 0, 0, WinAPIHandles::get_window_width(), WinAPIHandles::get_window_height(), m_buffer_hdc, 0, 0,
		       SRCCOPY);
	}
}
