#include <random>

#include "application.h"

#include "characterScene.h"
#include "input.hpp"
#include "deltatime.hpp"
#include "sceneManager.hpp"
#include "titleScene.h"

namespace Fortress
{
	void Application::initialize(const HWND hwnd, const HDC hdc)
	{
		m_hwnd = hwnd;
		m_hdc = hdc;
		Input::initialize();
		DeltaTime::initialize();
		Scene::SceneManager::initialize(hwnd, hdc);
		Scene::SceneManager::CreateScene<Scene::TitleScene>();
		Scene::SceneManager::CreateScene<Scene::CharacterScene>();
		Scene::SceneManager::SetActive(L"Title Scene");

		AdjustWindowRect(&m_window_size, WS_OVERLAPPEDWINDOW, false);
		SetWindowPos(m_hwnd, nullptr, 0, 0, get_window_width(),
		             get_window_height(), 0);
		ShowWindow(m_hwnd, true);
		UpdateWindow(m_hwnd);

		m_buffer_bitmap = CreateCompatibleBitmap(m_hdc, get_window_width(), get_window_height());
		m_buffer_hdc = CreateCompatibleDC(m_hdc);

		// m_buffer_hdc selects the buffer bitmap.
		const auto defaultBitmap = SelectObject(m_buffer_hdc, m_buffer_bitmap);
		// free the temporary handle.
		DeleteObject(defaultBitmap);
	}

	void Application::checkWindowFrame(Character& target)
	{
		const float topmenu_size = GetSystemMetrics(SM_CXFIXEDFRAME) * 2 + GetSystemMetrics(SM_CYMENU) + GetSystemMetrics(SM_CYCAPTION);
		const float r2 = target.m_hitbox.get_x();
		const float height = static_cast<float>(get_window_height());
		const float width = static_cast<float>(get_window_width());

		// hitting windows and character can happen individually.
		const auto newPos = target + target.m_velocity * DeltaTime::get_deltaTime();

		// Reflection vector
		// R = P + 2n(-P * n), where n = identity (= 1)
		if(target.get_x() <= 0)
		{
			target.m_velocity = target.m_velocity.reflect_x();
			target.m_position += {2.0f, 0.0f};
		}
		else if(target.get_x() >= width - r2)
		{
			target.m_velocity = target.m_velocity.reflect_x();
			target.m_position -= {2.0f, 0.0f};
		}
		if(target.get_y() <= 0)
		{
			target.m_velocity = target.m_velocity.reflect_y();
			target.m_position += {0.0f, 2.0f};
		}
		else if (target.get_y() >= height - topmenu_size - r2)
		{
			target.m_velocity = target.m_velocity.reflect_y();
			target.m_position -= {0.0f, 2.0f};
		}
	}

	void Application::update() const
	{
		if(!m_hdc || !m_hwnd || !m_buffer_hdc || !m_buffer_bitmap)
		{
			return;
		}

		DeltaTime::update();

		Input::update();
		Character::update();
		Scene::SceneManager::update();
	}

	void Application::render() const
	{
		const auto hbrBkGnd = CreateSolidBrush(GetSysColor(COLOR_WINDOW));
	    FillRect(m_buffer_hdc, &m_window_size, hbrBkGnd);
	    DeleteObject(hbrBkGnd);

		Scene::SceneManager::render();
		DeltaTime::render(m_buffer_hdc);

		BitBlt(m_hdc, 0 , 0, get_window_width(), get_window_height(), m_buffer_hdc, 0, 0, SRCCOPY);
	}
}
