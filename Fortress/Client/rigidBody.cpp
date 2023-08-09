#include "rigidBody.hpp"

#include "deltatime.hpp"
#include "math.h"
#include "sceneManager.hpp"

namespace Fortress::Abstract
{
	rigidBody::rigidBody(const std::wstring& name, const Math::Vector2& position, const Math::Vector2& hitbox,
	                            const Math::Vector2& velocity, const float& mass, const Math::Vector2& speed, const Math::Vector2& acceleration, 
								const bool& gravity) :
		object(name, position, hitbox, mass),
		m_velocity(velocity),
		m_acceleration(acceleration),
		m_movement_pitch_radian(0.0f),
		m_speed(speed),
		m_curr_speed{},
		m_bGravity(gravity),
		m_offset(Math::forward),
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
			std::shared_ptr rb = std::dynamic_pointer_cast<rigidBody>(locked_ptr);

			if (!rb || shared_from_this() == locked_ptr || !this->is_active() || !rb->is_active())
			{
				continue;
			}

			const auto objectified_this = std::dynamic_pointer_cast<object>(shared_from_this());

			// check collsion.
			CollisionCode code = is_collision(objectified_this, locked_ptr);

			if(code != CollisionCode::None)
			{
				collided = true;
				on_collision(
					code,
					to_hit_vector(get_center(), rb->get_center()),
					rb);
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

	void rigidBody::prerender()
	{
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
		m_offset = Math::bottom;
		m_velocity = {0, 1.0f};
	}

	void rigidBody::move_left()
	{
		m_offset = Math::left;
		m_velocity = {-1.0f, 0};
	}

	void rigidBody::move_right()
	{
		m_offset = Math::right;
		m_velocity = {1.0f, 0};
	}

	void rigidBody::move_up()
	{
		m_offset = Math::top;
		m_velocity = {0, -1.0f};
	}

	void rigidBody::stop()
	{
		m_velocity = {0, 0};
	}

	void rigidBody::set_movement_pitch_radian(const float pitch)
	{
		m_movement_pitch_radian = pitch;
	}

	void rigidBody::set_user_pitch_radian(const float pitch)
	{
		m_user_pitch_radian = pitch;
	}

	float rigidBody::get_movement_pitch_radian() const
	{
		return m_movement_pitch_radian;
	}

	float rigidBody::get_user_pitch_radian() const
	{
		return m_user_pitch_radian;
	}

	const Math::Vector2& rigidBody::get_offset() const
	{
		return m_offset;
	}

	Math::Vector2 rigidBody::get_mixed_offset() const
	{
		if(m_offset != Math::left || m_offset != Math::right)
		{
			return get_velocity_offset();
		}

		return m_offset;
	}

	Math::Vector2 rigidBody::get_velocity_offset() const
	{
		return m_velocity.get_x() < 0 ? Math::left : Math::right;
	}

	Math::Vector2 rigidBody::get_offset_forward_position() const
	{
		return get_offset() == Math::left ? get_left() : get_right();
	}

	Math::Vector2 rigidBody::get_offset_bottom_forward_position() const
	{
		return get_offset() == Math::left ? get_bottom_left() : get_bottom_right();
	}


	Math::Vector2 rigidBody::get_velocity_forward_position() const
	{
		return get_velocity_offset() == Math::left ? get_left() : get_right();
	}

	void rigidBody::set_offset(const Math::Vector2& offset)
	{
		m_offset = offset;
	}

	void rigidBody::on_collision(const CollisionCode& collision, const Math::Vector2& hit_vector, const std::weak_ptr<rigidBody>& other)
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

		const bool right_boundary_check = 
			left_locked_ptr->get_right().get_x() - right_locked_ptr->get_left().get_x() > Math::epsilon;

		const bool left_boundary_check = 
			left_locked_ptr->get_left().get_x() - right_locked_ptr->get_right().get_x() < Math::epsilon;

		const bool bottom_boundary_check = 
			left_locked_ptr->get_bottom().get_y() - right_locked_ptr->get_top().get_y() > Math::epsilon;

		const bool top_boundary_check = 
			left_locked_ptr->get_top().get_y() - right_locked_ptr->get_bottom().get_y() < Math::epsilon;

		const bool right_inside_check = 
			left_locked_ptr->get_right().get_x() - right_locked_ptr->get_left().get_x() > Math::epsilon + 1.0f;

		const bool left_inside_check = 
			left_locked_ptr->get_left().get_x() - right_locked_ptr->get_right().get_x() < Math::epsilon + 1.0f;

		const bool bottom_inside_check = 
			left_locked_ptr->get_bottom().get_y() - right_locked_ptr->get_top().get_y() > Math::epsilon + 1.0f;

		const bool top_inside_check = 
			left_locked_ptr->get_top().get_y() - right_locked_ptr->get_bottom().get_y() < Math::epsilon + 1.0f;

		if(right_inside_check && left_inside_check && bottom_inside_check && top_inside_check)
		{
			return CollisionCode::Inside;
		}
		if(right_boundary_check && left_boundary_check && bottom_boundary_check && top_boundary_check)
		{
			return CollisionCode::Boundary;
		}

		return CollisionCode::None;
	}

	void rigidBody::reset_current_speed()
	{
		m_curr_speed = {};
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

	void rigidBody::set_speed(const Math::Vector2& speed)
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
			m_gravity_speed = 0.0f;
			return;
		}

		if (m_gravity_speed <= max_gravity_speed) 
		{
			m_gravity_speed += m_gravity_acceleration * DeltaTime::get_deltaTime() * 0.5f;
			*this += {0.0f, m_gravity_speed * DeltaTime::get_deltaTime()};
			m_gravity_speed += m_gravity_acceleration * DeltaTime::get_deltaTime() * 0.5f;
		}
	}

	void rigidBody::move()
	{
		if (m_curr_speed.magnitude() < Math::epsilon)
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
	}
}
