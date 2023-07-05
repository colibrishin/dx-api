#include <random>

#include "application.h"
#include "input.hpp"
#include "deltatime.hpp"

namespace Fortress
{
	void Application::initialize(const HWND hwnd, const HDC hdc)
	{
		m_hwnd = hwnd;
		m_hdc = hdc;
		Input::initialize();
		DeltaTime::initialize();

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

		for(auto& m_object : m_objects)
		{
			static thread_local std::mt19937 generator(static_cast<unsigned int>(time(nullptr)));
			std:: uniform_int_distribution h_distribution(0, get_window_width());
			std:: uniform_int_distribution w_distribution(0, get_window_height());

			Vector2 random_pos = {static_cast<float>(w_distribution(generator)), static_cast<float>(h_distribution(generator))};
			m_object = {random_pos, {1.0f, 1.0f}, {20.0f, 20.0f}, 0, 0, CharacterType::CANNON};
		}
	}

	void Application::run()
	{
	}

	void Application::checkWindowFrame(Character& target)
	{
		const float topmenu_size = GetSystemMetrics(SM_CXFIXEDFRAME) * 2 + GetSystemMetrics(SM_CYMENU) + GetSystemMetrics(SM_CYCAPTION);
		const float r2 = target.m_hitbox.get_x();
		const float height = static_cast<float>(get_window_height());
		const float width = static_cast<float>(get_window_width());

		// hitting windows and character can happen individually.
		const auto newPos = target + target.m_velocity * speed * DeltaTime::get_deltaTime();

		// Reflection vector
		// R = P + 2n(-P * n), where n = identity (= 1)
		if(newPos.get_x() <= 0 || newPos.get_x() >= width - r2)
		{
			target.m_velocity += {target.m_velocity.get_x() * -2.0f, 0};
		}
		else if(newPos.get_y() <= 0 || newPos.get_y() >= height - topmenu_size - r2)
		{
			target.m_velocity += {0, target.m_velocity.get_y() * -2.0f};
		}
	}

	void Application::checkKeyUpdate()
	{
		Input::update();

		if (Input::getKey(eKeyCode::A))
		{
			m_playerPos = m_playerPos.left() * speed * DeltaTime::get_deltaTime();
		}
		if (Input::getKey(eKeyCode::D))
		{
			m_playerPos = m_playerPos.right() * speed * DeltaTime::get_deltaTime();
		}
		if (Input::getKey(eKeyCode::W))
		{
			// This is not a bug, value changing is in reverse.
			m_playerPos = m_playerPos.bottom() * speed * DeltaTime::get_deltaTime();
		}
		if (Input::getKey(eKeyCode::S))
		{
			m_playerPos = m_playerPos.top() * speed * DeltaTime::get_deltaTime();
		}
	}

	void Application::update()
	{
		if(!m_hdc || !m_hwnd || !m_buffer_hdc || !m_buffer_bitmap)
		{
			return;
		}

		DeltaTime::update();
		Character::update();

		static float interval = 0.0f;
		interval += DeltaTime::get_deltaTime();

		if(interval >= 0.01f)
		{
			for(auto& c : m_objects)
			{
				checkWindowFrame(c);

				// @todo: _rigidbody.move() should have do this trick.
				c += c.m_velocity * speed * DeltaTime::get_deltaTime();
			}
			interval = 0;
		}

		checkKeyUpdate();
	}

	void Application::render()
	{
		const auto hbrBkGnd = CreateSolidBrush(GetSysColor(COLOR_WINDOW));
	    FillRect(m_buffer_hdc, &m_window_size, hbrBkGnd);
	    DeleteObject(hbrBkGnd);

		DeltaTime::render(m_buffer_hdc);
		for(auto & m_object : m_objects)
		{
			Ellipse(
				m_buffer_hdc, 
				m_object.get_x(), 
				m_object.get_y(), 
				m_object.get_x() + m_object.m_hitbox.get_x(),
				m_object.get_y() + m_object.m_hitbox.get_y());
		}

		BitBlt(m_hdc, 0 , 0, get_window_width(), get_window_height(), m_buffer_hdc, 0, 0, SRCCOPY);
	}
}
