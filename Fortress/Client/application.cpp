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

		m_objects.resize(1);
		m_objects[0] = Character(
			{1.0f, 1.0f},
			{0.0f, 0.0f},
			{20.0f, 20.0f},
			500.0f,
			0.0f,
			0,
			0,
			CharacterType::CANNON);
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

	void Application::checkKeyUpdate()
	{
		Input::update();

		// @todo: diagonal move is janky.
		if (Input::getKey(eKeyCode::A))
		{
			m_objects[0].move_left();
		}
		if (Input::getKey(eKeyCode::D))
		{
			m_objects[0].move_right();
		}
		if (Input::getKey(eKeyCode::W))
		{
			m_objects[0].move_up();
		}
		if (Input::getKey(eKeyCode::S))
		{
			m_objects[0].move_down();
		}

		if(Input::getKeyUp(eKeyCode::A) || 
			Input::getKeyUp(eKeyCode::D) || 
			Input::getKeyUp(eKeyCode::W) || 
			Input::getKeyUp(eKeyCode::S))
		{
			m_objects[0].stop();
		}
	}

	void Application::update()
	{
		if(!m_hdc || !m_hwnd || !m_buffer_hdc || !m_buffer_bitmap)
		{
			return;
		}

		DeltaTime::update();

		checkKeyUpdate();

		checkWindowFrame(m_objects[0]);
		Character::update();
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
