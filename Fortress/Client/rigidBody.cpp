#include "rigidBody.hpp"

#include "deltatime.hpp"
#include "math.h"
#include "sceneManager.hpp"

namespace Fortress::Abstract
{
	rigidBody::rigidBody(const std::wstring& name, const Math::Vector2& position, const Math::Vector2& hitbox,
	                            const Math::Vector2& velocity, const float& mass, const float& speed, const float& acceleration, 
								const bool& gravity, const bool& clipping) :
		object(name, position, hitbox, mass),
		m_velocity(velocity),
		m_speed(speed),
		m_acceleration(acceleration),
		m_curr_speed(0.0f),
		m_bGravity(gravity),
		m_bClipping(clipping),
		m_gravity_speed(0.0f),
		m_gravity_acceleration(Math::G_ACC)
	{
		rigidBody::initialize();
	}

	void rigidBody::initialize()
	{
		object::initialize();
	}

	void rigidBody::update()
	{
		if (!is_active())
		{
			return;
		}

		bool collided = false;

		// gets the objects from active scene.
		for (const auto& right_r : Scene::SceneManager::get_active_scene().lock()->get_objects())
		{
			// @todo: performance degrading. objects could contain others.
			// check if object is rigid body.
			const auto locked_ptr = right_r.lock();
			std::shared_ptr<rigidBody> rb = std::dynamic_pointer_cast<rigidBody>(locked_ptr);

			if (!rb || shared_from_this() == locked_ptr || !this->is_active() || !rb->is_active())
			{
				continue;
			}

			// check collsion.
			CollisionCode code = is_collision(std::dynamic_pointer_cast<object>(shared_from_this()), locked_ptr);

			if(code != CollisionCode::None)
			{
				// if object is not reflecting each other, we don't update reflection.
				if(rb->m_bClipping)
				{
					update_reflection(code);
				}

				collided = true;

				on_collision(code, rb);
			}
		}

		if(!collided)
		{
			on_nocollison();
		}

		move();

		// if object is affected by gravity...
		if(m_bGravity)
		{
			apply_gravity();
		}
	}

	void rigidBody::block_window_frame(const std::weak_ptr<rigidBody>& target)
	{
		const auto locked_ptr = target.lock();

		if (locked_ptr->get_right().get_x() > WinAPIHandles::get_window_width() ||
			locked_ptr->get_bottom().get_y()> WinAPIHandles::get_actual_max_y())
		{
			// OOB
			// @todo: if the scene moves like "camera" then oob definition can be different with assumption.
			// @todo: if the character is oob in y-axis wise, then consider it as dead.
			locked_ptr->m_position = {1.0f, 1.0f};
			return;
		}

		if (locked_ptr->get_left().get_x() <= 0)
		{
			locked_ptr->m_velocity = locked_ptr->m_velocity.reflect_x();
			locked_ptr->m_position += {1.0f, 0.0f};
		}
		else if (locked_ptr->get_right().get_x() - WinAPIHandles::get_window_width() > Math::epsilon)
		{
			locked_ptr->m_velocity = locked_ptr->m_velocity.reflect_x();
			locked_ptr->m_position -= {1.0f, 0.0f};
		}

		if (locked_ptr->get_top().get_y() <= 0)
		{
			locked_ptr->m_velocity = locked_ptr->m_velocity.reflect_y();
			locked_ptr->m_position += {0.0f, 1.0f};
		}
		else if (locked_ptr->get_bottom().get_y() - WinAPIHandles::get_actual_max_y() > Math::epsilon)
		{
			locked_ptr->m_velocity = locked_ptr->m_velocity.reflect_y();
			locked_ptr->m_position -= {0.0f, 1.0f};
		}
	}

	void rigidBody::on_nocollison()
	{
	}

	void rigidBody::move_down()
	{
		m_velocity = {0, 1.0f};
	}

	void rigidBody::move_left()
	{
		m_velocity = {-1.0f, 0};
	}

	void rigidBody::move_right()
	{
		m_velocity = {1.0f, 0};
	}

	void rigidBody::move_up()
	{
		m_velocity = {0, -1.0f};
	}

	void rigidBody::stop()
	{
		m_velocity = {0, 0};
	}

	void rigidBody::on_collision(const CollisionCode& code, const std::shared_ptr<rigidBody>& other)
	{
		
	}


	CollisionCode rigidBody::is_collision(const std::weak_ptr<object>& left, const std::weak_ptr<object>& right) noexcept
	{
		const auto left_locked_ptr = left.lock();
		const auto right_locked_ptr = right.lock();

		const auto diff = left_locked_ptr->get_top_left() - right_locked_ptr->get_top_left();

		// identical
		if (left_locked_ptr == right_locked_ptr)
		{
			return CollisionCode::Identical;
		}

		const bool right_check = left_locked_ptr->get_right().get_x() >= right_locked_ptr->get_left().get_x();
		const bool left_check = left_locked_ptr->get_left().get_x() <= right_locked_ptr->get_right().get_x();
		const bool bottom_check = left_locked_ptr->get_bottom().get_y() >= right_locked_ptr->get_top().get_y();
		const bool top_check = left_locked_ptr->get_top().get_y() <= right_locked_ptr->get_bottom().get_y();

		if(right_check && left_check && bottom_check && top_check)
		{
			const float x = diff.normalized().get_x();
			const float y = diff.normalized().get_y();

			// using epsilon instead of zero.
			if (x < 0 && std::fabs(y) < Math::epsilon)
			{
				Debug::Log(L"Left");
				return CollisionCode::Left;
			}
			if (x > 0 && std::fabs(y) < Math::epsilon)
			{
				Debug::Log(L"Right");
				return CollisionCode::Right;
			}
			if (std::fabs(x) < Math::epsilon && y < 0)
			{
				Debug::Log(L"Bottom");
				return CollisionCode::Bottom;
			}
			if (std::fabs(x) < Math::epsilon && y > 0)
			{
				Debug::Log(L"Top");
				return CollisionCode::Top;
			}
			if (x < 0 && y < 0)
			{
				Debug::Log(L"BottomLeft");
				return CollisionCode::BottomLeft;
			}
			if (x > 0 && y > 0)
			{
				Debug::Log(L"TopRight");
				return CollisionCode::TopRight;
			}
			if (x > 0 && y < 0)
			{
				Debug::Log(L"BottomRight");
				return CollisionCode::BottomRight;
			}
			if (x < 0 && y > 0)
			{
				Debug::Log(L"TopLeft");
				return CollisionCode::TopLeft;
			}
		}

		Debug::Log(L"None");
		return CollisionCode::None;
	}

	void rigidBody::reset_current_speed()
	{
		Debug::Log(L"Speed reset at " + get_name());
		m_curr_speed = 0;
	}

	void rigidBody::reset_current_gravity_speed()
	{
		m_gravity_speed = 0;
	}

	void rigidBody::disable_gravity()
	{
		m_bGravity = false;
	}

	void rigidBody::enable_gravity()
	{
		m_bGravity = true;
	}

	void rigidBody::set_speed(const float speed)
	{
		m_speed = speed;
	}

	void rigidBody::apply_gravity()
	{
		// free-falling
		const Math::Vector2 diff = get_bottom() - Math::Vector2{
			0.0f, static_cast<float>(WinAPIHandles::get_actual_max_y())
		};

		if (diff.abs().get_y() <= Math::epsilon)
		{
			Debug::Log(L"Object grounded");
			m_gravity_speed = 0.0f;
			return;
		}

		Debug::Log(L"Object is floating");

		m_gravity_speed += m_gravity_acceleration * DeltaTime::get_deltaTime() * 0.5f;
		*this += {0.0f, get_mass() * m_gravity_speed * DeltaTime::get_deltaTime()};
		m_gravity_speed += m_gravity_acceleration * DeltaTime::get_deltaTime() * 0.5f;
	}

	bool rigidBody::update_reflection(CollisionCode code) noexcept
	{
		if (code == CollisionCode::None)
		{
			return false;
		}

		if (static_cast<unsigned char>(code) & static_cast<unsigned char>(CollisionCode::Left) ||
			static_cast<unsigned char>(code) & static_cast<unsigned char>(CollisionCode::Right))
		{
			m_velocity = m_velocity.reflect_x();
		}

		if (static_cast<unsigned char>(code) & static_cast<unsigned char>(CollisionCode::Top) ||
			static_cast<unsigned char>(code) & static_cast<unsigned char>(CollisionCode::Bottom))
		{
			m_velocity = m_velocity.reflect_y();
		}

		return true;
	}

	void rigidBody::move()
	{
		if (m_curr_speed < Math::epsilon)
		{
			m_curr_speed = m_speed;
		}

		if (m_velocity == Math::Vector2{0.0f, 0.0f})
		{
			reset_current_speed();
			return;
		}

		//@todo: friction.
		m_curr_speed += m_acceleration * DeltaTime::get_deltaTime() * 0.5f;
		*this += m_velocity * m_curr_speed * DeltaTime::get_deltaTime();
		m_curr_speed += m_acceleration * DeltaTime::get_deltaTime() * 0.5f;

		Debug::Log(L"Moving " + get_name() + std::to_wstring(m_curr_speed));
	}
}
