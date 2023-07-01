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
	}

	void Application::run()
	{
	}

	void Application::moveRandomly()
	{
		static float interval = 0.0f;

		interval += DeltaTime::get_deltaTime();
		if (interval >= 0.01f)
		{
			constexpr float offset[8][2] = 
			{
				{-1.0f, 0.0f},
				{1.0f, 0.0f},
				{0.0f, -1.0f},
				{0.0f, 1.0f},
				{-1.0f, -1.0f},
				{-1.0f, 1.0f},
				{1.0f, -1.0f},
				{1.0f, 1.0f}
			};

			constexpr int offset_length = sizeof offset / sizeof(int) / 2;
			static thread_local std::mt19937 generator(static_cast<unsigned int>(time(nullptr)));
			std:: uniform_int_distribution distribution(0, offset_length);

			const int random_offset_idx = distribution(generator);

			const float x = offset[random_offset_idx][0] * 2.0f + (offset[random_offset_idx][0] > 0.0f ? DeltaTime::get_deltaTime() : 0.0f);
			const float y = offset[random_offset_idx][1] * 2.0f + (offset[random_offset_idx][1] > 0.0f ? DeltaTime::get_deltaTime() : 0.0f);

			RECT rect;
			if(GetWindowRect(m_hwnd, &rect))
			{
				const float height = rect.bottom - rect.top;
				const float width = rect.right - rect.left;

				const Vector2 newPos = m_playerPos + Vector2{x, y};
				if(newPos.get_x() >= 0 && newPos.get_x() <= width && newPos.get_y() <= height && newPos.get_y() >= 0)
				{
					m_playerPos = newPos;
					m_update_tick.set_ticked();
				}
				interval = 0.0f;
			}
		}
		
	}

	void Application::reflectiveMove()
	{
		static float interval = 0.0f;

		interval += DeltaTime::get_deltaTime();

		if(interval >= 0.01f)
		{
			RECT rect;
			if(GetWindowRect(m_hwnd, &rect))
			{
				static Vector2 velocity{2.0f, 2.0f};
				const float top_tab_size = GetSystemMetrics(SM_CXFIXEDFRAME) * 2 + GetSystemMetrics(SM_CYMENU) + GetSystemMetrics(SM_CYCAPTION);
				constexpr float r2 = 2 * 6.0f;
				const float height = rect.bottom - rect.top;
				const float width = rect.right - rect.left;

				const Vector2 newPos = m_playerPos + velocity;
				// Reflection vector
				// R = P + 2n(-P * n), where n = identity (= 1)
				if(newPos.get_x() <= 0 || newPos.get_x() >= width - r2)
				{
					velocity += {velocity.get_x() * -2.0f, 0};
				}
				else if(newPos.get_y() <= 0 || newPos.get_y() >= height - top_tab_size - r2)
				{
					velocity += {0, velocity.get_y() * -2.0f};
				}

				m_playerPos += velocity;
				m_update_tick.set_ticked();
				interval = 0.0f;
			}
		}
	}

	void Application::checkKeyUpdate()
	{
		Input::update();

		if (Input::getKey(eKeyCode::A))
		{
			m_playerPos = m_playerPos.left() + Vector2{-DeltaTime::get_deltaTime(), 0};
			m_update_tick.set_ticked();
		}
		if (Input::getKey(eKeyCode::D))
		{
			m_playerPos = m_playerPos.right() + Vector2{DeltaTime::get_deltaTime(), 0};
			m_update_tick.set_ticked();
		}
		if (Input::getKey(eKeyCode::W))
		{
			// This is not a bug, value changing is in reverse.
			m_playerPos = m_playerPos.bottom() + Vector2{0, -DeltaTime::get_deltaTime()};
			m_update_tick.set_ticked();
		}
		if (Input::getKey(eKeyCode::S))
		{
			m_playerPos = m_playerPos.top() + Vector2{0, DeltaTime::get_deltaTime()};
			m_update_tick.set_ticked();
		}
	}

	void Application::update()
	{
		if(!m_hdc)
		{
			return;
		}

		if(!m_hwnd)
		{
			return;
		}

		DeltaTime::update();

		//moveRandomly();
		reflectiveMove();
		checkKeyUpdate();
		if (m_update_tick.is_ticked())
		{
			InvalidateRect(m_hwnd, nullptr, true);
			m_update_tick.unset_ticked();
		}
	}

	void Application::render()
	{
		DeltaTime::render(m_hdc);
		Ellipse(m_hdc, m_playerPos.get_x(), m_playerPos.get_y(), m_playerPos.get_x() + 6 ,m_playerPos.get_y() + 6);
	}
}
