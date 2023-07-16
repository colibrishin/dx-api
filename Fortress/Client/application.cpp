#include <random>

#include "application.h"

#include "BattleScene.h"
#include "BulletinBoardScene.h"
#include "input.hpp"
#include "deltatime.hpp"
#include "LobbyScene.h"
#include "RoomScene.h"
#include "sceneManager.hpp"
#include "titleScene.h"
#include "winapihandles.hpp"

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
		Scene::SceneManager::CreateScene<Scene::BattleScene>();
		Scene::SceneManager::CreateScene<Scene::BulletinBoardScene>();
		Scene::SceneManager::CreateScene<Scene::LobbyScene>();
		Scene::SceneManager::CreateScene<Scene::RoomScene>();
		Scene::SceneManager::SetActive(L"Title Scene");
	}

	void Application::update()
	{
		if (!m_hdc || !m_hwnd || !m_buffer_hdc)
		{
			return;
		}

		/*Debug::draw_line(
			{static_cast<float>(WinAPIHandles::get_window_width() / 2), 0}, 
			{static_cast<float>(WinAPIHandles::get_window_width() / 2), static_cast<float>(WinAPIHandles::get_actual_max_y())});

		Debug::draw_line(
			{static_cast<float>(0), static_cast<float>(WinAPIHandles::get_actual_max_y() / 2)}, 
			{static_cast<float>(WinAPIHandles::get_window_width()), static_cast<float>(WinAPIHandles::get_actual_max_y() / 2)})*/;

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
		Scene::SceneManager::render();
		Debug::render();

		BitBlt(m_hdc, 0, 0, WinAPIHandles::get_window_width(), WinAPIHandles::get_window_height(), m_buffer_hdc, 0, 0,
		       SRCCOPY);
	}
}
