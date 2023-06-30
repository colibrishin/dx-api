#include <cmath>

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
		DeltaTime::update();
		checkKeyUpdate();
		DeltaTime::render(m_hdc);

		// @todo: test with static push
		m_render_queue.push([this]()
		{
			TextOut(m_hdc, m_playerPos.get_x(), m_playerPos.get_y(), L"@", 1);
		});

		if (m_update_tick.is_ticked())
		{
			InvalidateRect(m_hwnd, nullptr, true);
			m_update_tick.unset_ticked();
		}
	}

	void Application::render()
	{
		if (!m_render_tick.is_ticked())
		{
			m_render_tick.set_ticked();

			while (!m_render_queue.empty())
			{
				m_render_queue.front()();
				m_render_queue.pop();
			}

			m_render_tick.unset_ticked();
		}
	}
}
