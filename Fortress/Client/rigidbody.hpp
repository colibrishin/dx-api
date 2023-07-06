#pragma once
#ifndef RIGIDBODY_HPP
#define RIGIDBODY_HPP

#include <vector>
#include <string>
#include "deltatime.hpp"
#include "math.h"
#include "debug.hpp"
#include "object.hpp"
#include "winapihandles.hpp"

namespace ObjectInternal
{
	enum class CollisionCode
	{
		None = 0,
		Identical = 1,
		Top = 2,
		Bottom = 4,
		Left = 8,
		Right = 16,
		TopLeft = 10,
		TopRight = 18,
		BottomLeft = 12,
		BottomRight = 20,
	};

	class _rigidBody : public _baseObject
	{
	public:
		Math::Vector2 m_velocity;
		float m_speed;
		float m_acceleration;
		bool m_bActive;
		bool m_bGrounded;

		_rigidBody() = delete;
		_rigidBody& operator=(const _rigidBody& other);
		__forceinline void initialize();
		__forceinline static void update();
		__forceinline static void block_window_frame(_rigidBody& target);
		~_rigidBody() override;
		void move_down() override;
		void move_left() override;
		void move_right() override;
		void move_up() override;
		void stop();

	private:
		float m_curr_speed;
		float m_gravity_speed;
		float m_gravity_acceleration;

		static void apply_gravity(_rigidBody* obj);
		__forceinline static void update_collision(_rigidBody* left, const _rigidBody* right) noexcept;
		__forceinline static CollisionCode is_collision(const _rigidBody* left, const _rigidBody* right) noexcept;
		__forceinline static void move(_rigidBody* object);
		inline static std::vector<_rigidBody*> _known_rigid_bodies = {};

	protected:
		_rigidBody(const std::wstring& name, Math::Vector2 position, Math::Vector2 hitbox, Math::Vector2 velocity,
		           float speed,
		           float acceleration);
	};

	inline _rigidBody& _rigidBody::operator=(const _rigidBody& other)
	{
		m_position = other.m_position;
		m_velocity = other.m_velocity;
		m_hitbox = other.m_hitbox;
		m_speed = other.m_speed;
		m_acceleration = other.m_acceleration;
		m_bActive = other.m_bActive;
		m_gravity_speed = other.m_gravity_speed;
		m_gravity_acceleration = other.m_gravity_acceleration;
		m_bGrounded = other.m_bGrounded;
		return *this;
	}

	inline _rigidBody::_rigidBody(const std::wstring& name, const Math::Vector2 position, const Math::Vector2 hitbox,
		const Math::Vector2 velocity, const float speed, const float acceleration) :
		_baseObject(name, position, hitbox),
		m_velocity(velocity),
		m_speed(speed),
		m_acceleration(acceleration),
		m_curr_speed(0.0f),
		m_bActive(true),
		m_gravity_speed(0.0f),
		m_gravity_acceleration(Math::G_ACC),
		m_bGrounded(false)
	{
		initialize();
	}

	__forceinline void _rigidBody::initialize()
	{
		_known_rigid_bodies.push_back(this);
	}

	__forceinline void _rigidBody::update()
	{
		for (const auto left_r : _known_rigid_bodies)
		{
			for (const auto right_r : _known_rigid_bodies)
			{
				if (left_r == right_r || !left_r->m_bActive || !right_r->m_bActive)
				{
					continue;
				}

				update_collision(left_r, right_r);
			}
		}

		for (const auto& r : _known_rigid_bodies)
		{
			if(!r->m_bActive)
			{
				continue;
			}

			move(r);
			apply_gravity(r);
		}
	}

	inline void _rigidBody::block_window_frame(_rigidBody& target)
	{
		const float x = target.m_hitbox.get_x() * 2;
		const float y = target.m_hitbox.get_y();

		if(target.get_x() > WinAPIHandles::get_window_width() || 
			target.get_y() > WinAPIHandles::get_actual_max_y())
		{
			// OOB
			target.m_position = {1.0f, 1.0f};
			return;
		}

		if(target.get_x() <= 0)
		{
			target.m_velocity = target.m_velocity.reflect_x();
			target.m_position += {1.0f, 0.0f};
		}
		else if(target.get_x() - (WinAPIHandles::get_window_width() - x) > Math::epsilon)
		{
			target.m_velocity = target.m_velocity.reflect_x();
			target.m_position -= {1.0f, 0.0f};
		}

		if(target.get_y() <= 0)
		{
			target.m_velocity = target.m_velocity.reflect_y();
			target.m_position += {0.0f, 1.0f};
		}
		else if (target.get_y() - (WinAPIHandles::get_actual_max_y() - y) > Math::epsilon)
		{
			target.m_velocity = target.m_velocity.reflect_y();
			target.m_position -= {0.0f, 1.0f};
		}
	}

	inline _rigidBody::~_rigidBody()
	{
		_known_rigid_bodies.erase(
			std::remove_if(
				_known_rigid_bodies.begin(),
				_known_rigid_bodies.end(),
				[this](const _rigidBody* r)
				{
					return r == this;
				}),
			_known_rigid_bodies.end());
	}

	inline void _rigidBody::move_down()
	{
		m_velocity = {0, 1.0f};
	}

	inline void _rigidBody::move_left()
	{
		m_velocity = {-1.0f, 0};
	}

	inline void _rigidBody::move_right()
	{
		m_velocity = {1.0f, 0};
	}

	inline void _rigidBody::move_up()
	{
		m_velocity = {0, -1.0f};
	}

	inline void _rigidBody::stop()
	{
		m_velocity = {0, 0};
	}
	
	inline void _rigidBody::apply_gravity(_rigidBody* obj)
	{
		// @todo: set ground position dynamically
		const auto diff = obj->m_position - Math::Vector2{0.0f, static_cast<float>(WinAPIHandles::get_actual_max_y())};

		if(std::floorf(diff.abs().get_y() - obj->m_hitbox.get_y()) == 0)
		{
			obj->m_bGrounded = true;
			obj->m_gravity_speed = 0.0f;
		}
		else
		{
			obj->m_bGrounded = false;
		}

		if(!obj->m_bGrounded)
		{
			obj->m_gravity_speed += obj->m_gravity_acceleration * Fortress::DeltaTime::get_deltaTime() * 0.5f;
			*obj += {0.0f, obj->m_gravity_speed * Fortress::DeltaTime::get_deltaTime()};
			obj->m_gravity_speed += obj->m_gravity_acceleration * Fortress::DeltaTime::get_deltaTime() * 0.5f;
		}
	}

	__forceinline void _rigidBody::update_collision(_rigidBody* left, const _rigidBody* right) noexcept
	{
		CollisionCode code = is_collision(left, right);

		if (code == CollisionCode::None)
		{
			return;
		}

		if (code == CollisionCode::Identical)
		{
			left->m_velocity = -left->m_velocity;
			return;
		}

		if (static_cast<unsigned char>(code) & static_cast<unsigned char>(CollisionCode::Left) ||
			static_cast<unsigned char>(code) & static_cast<unsigned char>(CollisionCode::Right))
		{
			left->m_velocity = left->m_velocity.reflect_x();
		}

		if (static_cast<unsigned char>(code) & static_cast<unsigned char>(CollisionCode::Top) ||
			static_cast<unsigned char>(code) & static_cast<unsigned char>(CollisionCode::Bottom))
		{
			left->m_velocity = left->m_velocity.reflect_y();
		}
	}

	__forceinline CollisionCode _rigidBody::is_collision(const _rigidBody* left, const _rigidBody* right) noexcept
	{
		// See also: https://www.acmicpc.net/problem/1002

		const auto diff = left->get_position() - right->get_position();
		const auto dist = std::sqrtf(std::powf(diff.get_x(), 2) + std::powf(diff.get_y(), 2));

		// @todo: if two objects intersect, their collisions are done indefinitely.
		// identical
		if (left->m_position == right->get_position())
		{
			return CollisionCode::Identical;
		}

		const auto hitbox_diff = (left->m_hitbox - right->m_hitbox).abs();
		// @note: using one hitbox size due to winapi middle point is actually top left.
		//const auto hitbox_sum = m_hitbox + object.m_hitbox;
		const auto& hitbox_sum = left->m_hitbox;

		// Too far
		if (hitbox_diff.get_x() > dist || hitbox_sum.get_x() < dist ||
			hitbox_diff.get_y() > dist || hitbox_sum.get_y() < dist)
		{
			return CollisionCode::None;
		}

		float x = 0;
		float y = 0;

		// X Collision, meet each other in radius or meet each other their inside.
		if (hitbox_diff.get_x() - dist < Math::epsilon || hitbox_sum.get_x() - dist < Math::epsilon ||
			hitbox_diff.get_x() < dist && dist < hitbox_sum.get_x())
		{
			x = diff.unit_vector().get_x();
		}

		if (hitbox_diff.get_y() - dist < Math::epsilon || hitbox_sum.get_y() - dist < Math::epsilon ||
			hitbox_diff.get_y() < dist && dist < hitbox_sum.get_y())
		{
			y = diff.unit_vector().get_y();
		}

		if (std::fabs(x) < Math::epsilon && std::fabs(y) < Math::epsilon)
		{
			return CollisionCode::None;
		}

		// using epsilon instead of zero.
		if (x < 0 && std::fabs(y) < Math::epsilon)
		{
			Fortress::Debug::Log(L"Left");
			return CollisionCode::Left;
		}
		if (x > 0 && std::fabs(y) < Math::epsilon)
		{
			Fortress::Debug::Log(L"Right");
			return CollisionCode::Right;
		}
		if (std::fabs(x) < Math::epsilon && y < 0)
		{
			Fortress::Debug::Log(L"Bottom");
			return CollisionCode::Bottom;
		}
		if (std::fabs(x) < Math::epsilon && y > 0)
		{
			Fortress::Debug::Log(L"Top");
			return CollisionCode::Top;
		}
		if (x < 0 && y < 0)
		{
			Fortress::Debug::Log(L"BottomLeft");
			return CollisionCode::BottomLeft;
		}
		if (x > 0 && y > 0)
		{
			Fortress::Debug::Log(L"TopRight");
			return CollisionCode::TopRight;
		}
		if (x > 0 && y < 0)
		{
			Fortress::Debug::Log(L"BottomRight");
			return CollisionCode::BottomRight;
		}
		if (x < 0 && y > 0)
		{
			Fortress::Debug::Log(L"TopLeft");
			return CollisionCode::TopLeft;
		}

		return CollisionCode::None;
	}

	inline void _rigidBody::move(_rigidBody* object)
	{
		if (object->m_curr_speed < Math::epsilon)
		{
			object->m_curr_speed = object->m_speed;
		}

		if (object->m_velocity == Math::Vector2{0.0f, 0.0f})
		{
			object->m_curr_speed = 0;
			return;
		}

		//@todo: gravity, friction.
		object->m_curr_speed += object->m_acceleration * Fortress::DeltaTime::get_deltaTime() * 0.5f;
		*object += object->m_velocity * object->m_curr_speed * Fortress::DeltaTime::get_deltaTime();
		object->m_curr_speed += object->m_acceleration * Fortress::DeltaTime::get_deltaTime() * 0.5f;
	}
}
#endif // RIGIDBODY_HPP
