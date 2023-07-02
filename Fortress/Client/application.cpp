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

		RECT rect;
		if(GetWindowRect(m_hwnd, &rect))
		{
			const int height = rect.bottom - rect.top;
			const int width = rect.right - rect.left;

			for(auto& m_object : m_objects)
			{
				static thread_local std::mt19937 generator(static_cast<unsigned int>(time(nullptr)));
				std:: uniform_int_distribution h_distribution(0, height);
				std:: uniform_int_distribution w_distribution(0, width);

				Vector2 random_pos = {static_cast<float>(w_distribution(generator)), static_cast<float>(h_distribution(generator))};
				m_object = {random_pos, {2.0f,2.0f}, {10, 10}, 0, 0, CharacterType::CANNON};
			}
		}
	}

	void Application::run()
	{
	}

	bool Application::updateCharacterCollision(Character& target)
	{
		Character updated = target;
		updated += target.m_velocity;
		bool collision = false;

		for(auto& object : m_objects)
		{
			if(&object == &target)
			{
				continue;
			}

			switch(target.is_collision(object))
			{
			case CollisionCode::Identical:
				target.m_velocity = -target.m_velocity;
				object.m_velocity = -object.m_velocity;
				collision = true;
				break;
			case CollisionCode::XHitInside:
			case CollisionCode::XHitBoundary:
				target.m_velocity += {target.m_velocity.get_x() * -2.0f, 0};
				object.m_velocity += {object.m_velocity.get_x() * -2.0f, 0};
				collision = true;
				break;
			case CollisionCode::YHitBoundary:
			case CollisionCode::YHitInside:
				target.m_velocity += {0, target.m_velocity.get_y() * -2.0f};
				object.m_velocity +=  {0, object.m_velocity.get_y() * -2.0f};
				collision = true;
				break;
			case CollisionCode::None:
			default:
				collision = false;
				break;
			}
		}
		return collision;
	}

	void Application::reflectiveMove(Character& target)
	{
		RECT rect;
		if(GetWindowRect(m_hwnd, &rect))
		{
			const float topmenu_size = GetSystemMetrics(SM_CXFIXEDFRAME) * 2 + GetSystemMetrics(SM_CYMENU) + GetSystemMetrics(SM_CYCAPTION);
			constexpr float r2 = 20.0f;
			const float height = rect.bottom - rect.top;
			const float width = rect.right - rect.left;

			if(!updateCharacterCollision(target))
			{
				const auto newPos = target + target.m_velocity;

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

			target += target.m_velocity + Vector2(DeltaTime::get_deltaTime(), DeltaTime::get_deltaTime());
			m_update_tick.set_ticked();
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

		static float interval = 0.0f;
		interval += DeltaTime::get_deltaTime();

		if(interval >= 0.01f)
		{
			for(auto& c : m_objects)
			{
				reflectiveMove(c);
			}
			interval = 0;
		}

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
		for(auto & m_object : m_objects)
		{
			Ellipse(m_hdc, m_object.get_x(), m_object.get_y(), m_object.get_x() + 20 ,m_object.get_y() + 20);
		}
	}
}
