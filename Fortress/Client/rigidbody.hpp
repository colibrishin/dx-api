#pragma once
#ifndef RIGIDBODY_HPP
#define RIGIDBODY_HPP

#include <vector>
#include <string>
#include "deltatime.hpp"
#include "math.h"
#include "object.hpp"

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

		_rigidBody() = delete;
		_rigidBody& operator=(const _rigidBody& other);
		__forceinline void initialize();
		__forceinline static void update();
		virtual ~_rigidBody() override;

	private:
		float m_curr_speed;
		__forceinline static void update_collision(_rigidBody* left, const _rigidBody* right) noexcept;
		__forceinline static CollisionCode is_collision(const _rigidBody* left, const _rigidBody* right) noexcept;
		__forceinline static void move(_rigidBody* object);
		inline static std::vector<_rigidBody*> _known_rigid_bodies = {};
	protected:
		_rigidBody(std::wstring& name, Math::Vector2 position, Math::Vector2 hitbox, Math::Vector2 velocity, float speed, float acceleration);
	};

	inline _rigidBody& _rigidBody::operator=(const _rigidBody& other)
	{
		m_position = other.m_position;
		m_velocity = other.m_velocity;
		m_hitbox = other.m_hitbox;
		m_speed = other.m_speed;
		m_acceleration = other.m_acceleration;
		return *this;
	}

	inline _rigidBody::_rigidBody(const std::wstring& name, const Math::Vector2 position, const Math::Vector2 hitbox,
		const Math::Vector2 velocity, const float speed, const float acceleration) :
		_baseObject(name, position, hitbox), m_velocity(velocity), m_speed(speed), m_acceleration(acceleration), m_curr_speed(0.0f)
	{
		initialize();
	}

	__forceinline void _rigidBody::initialize()
	{
		_known_rigid_bodies.push_back(this);
	}

	__forceinline void _rigidBody::update()
	{
		for(const auto left_r : _known_rigid_bodies)
		{
			for(const auto right_r : _known_rigid_bodies)
			{
				if(left_r == right_r)
				{
					continue;
				}

				update_collision(left_r, right_r);
			}
		}

		for(const auto& r : _known_rigid_bodies)
		{
			move(r);
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

	__forceinline void _rigidBody::update_collision(_rigidBody* left, const _rigidBody* right) noexcept
	{
		CollisionCode code = is_collision(left, right);

		if(code == CollisionCode::None)
		{
			return;
		}

		if(code == CollisionCode::Identical)
		{
			left->m_velocity = -left->m_velocity;
			return;
		}

		if((unsigned char)code & (unsigned char)CollisionCode::Left ||
			(unsigned char)code & (unsigned char)CollisionCode::Right)
		{
			left->m_velocity = left->m_velocity.reflect_x();
		}

		if((unsigned char)code & (unsigned char)CollisionCode::Top ||
			(unsigned char)code & (unsigned char)CollisionCode::Bottom)
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
	    if(left->m_position == right->get_position())
	    {
		    return CollisionCode::Identical;
	    }

	    const auto hitbox_diff = (left->m_hitbox - right->m_hitbox).abs();
	    // @note: using one hitbox size due to winapi middle point is actually top left.
	    //const auto hitbox_sum = m_hitbox + object.m_hitbox;
	    const auto& hitbox_sum = left->m_hitbox;

	    // Too far
	    if(hitbox_diff.get_x() > dist || hitbox_sum.get_x() < dist ||
	        hitbox_diff.get_y() > dist || hitbox_sum.get_y() < dist)
	    {
		    return CollisionCode::None;
	    }

		int x = 0;
		int y = 0;

	    // X Collision, meet each other in radius or meet each other their inside.
	    if(hitbox_diff.get_x() - dist < Math::epsilon || hitbox_sum.get_x() - dist < Math::epsilon || 
			hitbox_diff.get_x() < dist && dist < hitbox_sum.get_x())
	    {
			x = static_cast<int>(diff.unit_vector().get_x());
	    }

	    if (hitbox_diff.get_y() - dist < Math::epsilon || hitbox_sum.get_y() - dist < Math::epsilon || 
			hitbox_diff.get_y() < dist && dist < hitbox_sum.get_y())
	    {
			y = static_cast<int>(diff.unit_vector().get_y());
	    }

		if(!x && !y)
		{
			return CollisionCode::None;
		}

		if(x < 0 && !y)
		{
			return CollisionCode::Left;
		}
		if(x > 0 && !y)
		{
			return CollisionCode::Right;
		}
		if(!x && y < 0)
		{
			return CollisionCode::Bottom;
		}
		if(!x && y > 0)
		{
			return CollisionCode::Top;
		}
		if(x < 0 && y < 0)
		{
			return CollisionCode::BottomLeft;
		}
		if(x > 0 && y > 0)
		{
			return CollisionCode::TopRight;
		}
		if(x > 0 && y < 0)
		{
			return CollisionCode::BottomRight;
		}
		if(x < 0 && y > 0)
		{
			return CollisionCode::TopLeft;
		}
		

	    return CollisionCode::None;
	}

	inline void _rigidBody::move(_rigidBody* object)
	{
		if(object->m_curr_speed == 0.0f)
		{
			object->m_curr_speed = object->m_speed;
		}

		if(object->m_velocity == Math::Vector2{0.0f, 0.0f})
		{
			if(object->m_curr_speed > Math::epsilon)
			{
				object->m_curr_speed /= 4.0f;
			}
			else if(object->m_curr_speed < Math::epsilon)
			{
				object->m_curr_speed = 0;
				return;
			}
		}

		object->m_curr_speed += object->m_acceleration * Fortress::DeltaTime::get_deltaTime() * 0.5f;
		*object += object->m_velocity * object->m_curr_speed * Fortress::DeltaTime::get_deltaTime();
		object->m_curr_speed += object->m_acceleration * Fortress::DeltaTime::get_deltaTime() * 0.5f;
	}
}
#endif // RIGIDBODY_HPP
