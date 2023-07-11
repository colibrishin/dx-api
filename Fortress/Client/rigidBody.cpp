#include "rigidBody.hpp"

#include "deltatime.hpp"
#include "math.h"
#include "sceneManager.hpp"

namespace Fortress::Abstract
{
	rigidBody::rigidBody(const std::wstring& name, const Math::Vector2 position, const Math::Vector2 hitbox,
	                            const Math::Vector2 velocity, const float speed, const float acceleration, 
								const bool gravity, const bool clipping) :
		object(name, position, hitbox),
		m_velocity(velocity),
		m_speed(speed),
		m_acceleration(acceleration),
		m_curr_speed(0.0f),
		m_gravity_speed(0.0f),
		m_gravity_acceleration(Math::G_ACC),
		m_bGravity(gravity),
		m_bClipping(clipping)
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

		// gets the objects from active scene.
		for (const auto right_r : Scene::SceneManager::get_active_scene()->get_objects())
		{
			// @todo: performance degrading. objects could contain others.
			// check if object is rigid body.
			rigidBody* rb = dynamic_cast<rigidBody*>(right_r);

			if (!rb || this == right_r || !this->is_active() || !rb->is_active())
			{
				continue;
			}

			// check collsion.
			CollisionCode code = is_collision(this, right_r);

			if(code != CollisionCode::None)
			{
				// if object is not reflecting each other, we don't update reflection.
				if(rb->m_bClipping)
				{
					update_reflection(code);
				}

				on_collision(rb);
			}
		}

		move();

		// if object is affected by gravity...
		if(m_bGravity)
		{
			apply_gravity();
		}
	}

	void rigidBody::block_window_frame(rigidBody* target)
	{
		const float x = target->m_hitbox.get_x() * 2;
		const float y = target->m_hitbox.get_y();

		if (target->get_x() > WinAPIHandles::get_window_width() ||
			target->get_y() > WinAPIHandles::get_actual_max_y())
		{
			// OOB
			// @todo: if the scene moves like "camera" then oob definition can be different with assumption.
			// @todo: if the character is oob in y-axis wise, then consider it as dead.
			target->m_position = {1.0f, 1.0f};
			return;
		}

		if (target->get_x() <= 0)
		{
			target->m_velocity = target->m_velocity.reflect_x();
			target->m_position += {1.0f, 0.0f};
		}
		else if (target->get_x() - (WinAPIHandles::get_window_width() - x) > Math::epsilon)
		{
			target->m_velocity = target->m_velocity.reflect_x();
			target->m_position -= {1.0f, 0.0f};
		}

		if (target->get_y() <= 0)
		{
			target->m_velocity = target->m_velocity.reflect_y();
			target->m_position += {0.0f, 1.0f};
		}
		else if (target->get_y() - (WinAPIHandles::get_actual_max_y() - y) > Math::epsilon)
		{
			target->m_velocity = target->m_velocity.reflect_y();
			target->m_position -= {0.0f, 1.0f};
		}
	}

	rigidBody::~rigidBody()
	{
		object::~object();
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

	void rigidBody::on_collision(rigidBody* other)
	{
		
	}


	CollisionCode rigidBody::is_collision(const object* left, const object* right) noexcept
	{
		const auto left_mid = Math::Vector2{
			left->get_x() + left->m_hitbox.get_x() / 2,
			left->get_y() + left->m_hitbox.get_y() / 2};

		const auto right_mid = Math::Vector2{
			right->get_x() + left->m_hitbox.get_x() / 2,
			right->get_y() + left->m_hitbox.get_y() / 2};

		const auto diff = left_mid - right_mid;

		// identical
		if (left_mid == right_mid)
		{
			return CollisionCode::Identical;
		}

		if (left_mid.get_x() + left->m_hitbox.get_x() > right_mid.get_x() && left_mid.get_x() < right_mid.get_x() + right->m_hitbox.get_x() &&
			left_mid.get_y() + left->m_hitbox.get_y() > right_mid.get_y() && left_mid.get_y() < right_mid.get_y() + right->m_hitbox.get_y())
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

	void rigidBody::apply_gravity()
	{
		// free-falling
		const Math::Vector2 diff = m_position - Math::Vector2{
			0.0f, static_cast<float>(WinAPIHandles::get_actual_max_y())
		};

		if (std::floorf(diff.abs().get_y() - m_hitbox.get_y()) <= 0)
		{
			Debug::Log(L"Object grounded");
			m_gravity_speed = 0.0f;
			return;
		}

		Debug::Log(L"Object is floating");

		m_gravity_speed += m_gravity_acceleration * DeltaTime::get_deltaTime() * 0.5f;
		*this += {0.0f, m_gravity_speed * DeltaTime::get_deltaTime()};
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
			m_curr_speed = 0;
			return;
		}

		//@todo: friction.
		m_curr_speed += m_acceleration * DeltaTime::get_deltaTime() * 0.5f;
		*this += m_velocity * m_curr_speed * DeltaTime::get_deltaTime();
		m_curr_speed += m_acceleration * DeltaTime::get_deltaTime() * 0.5f;
	}
}
