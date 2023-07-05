#pragma once
#ifndef RIGIDBODY_HPP
#define RIGIDBODY_HPP

#include <vector>

#include "deltatime.hpp"
#include "math.h"
#include "object.hpp"

namespace ObjectInternal
{
	enum class CollisionCode
	{
		None = 0,
		Identical,
		XHitBoundary,
		XHitInside,
		YHitBoundary,
		YHitInside
	};

	class _rigidBody : public _baseObject
	{
	public:
		Math::Vector2 m_velocity;

		_rigidBody() = delete;
		_rigidBody& operator=(const _rigidBody& other);
		__forceinline void initialize();
		__forceinline static void update();
		virtual ~_rigidBody() override;

	private:
		__forceinline static void update_collision(_rigidBody* left, const _rigidBody* right) noexcept;
		__forceinline static CollisionCode is_collision(const _rigidBody* left, const _rigidBody* right) noexcept;
		__forceinline static void move(_rigidBody& object);
		inline static std::vector<_rigidBody*> _known_rigid_bodies = {};
	protected:
		_rigidBody(const Math::Vector2 position, const Math::Vector2 hitbox, const Math::Vector2 velocity);
	};

	inline _rigidBody& _rigidBody::operator=(const _rigidBody& other)
	{
		m_position = other.m_position;
		m_velocity = other.m_velocity;
		m_hitbox = other.m_hitbox;
		return *this;
	}

	inline _rigidBody::_rigidBody(const Math::Vector2 position, const Math::Vector2 hitbox,
		const Math::Vector2 velocity) :
		_baseObject(position, hitbox), m_velocity(velocity)
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
		switch(is_collision(left, right))
		{
		case CollisionCode::Identical:
			left->m_velocity = -left->m_velocity;
			break;
		case CollisionCode::XHitInside:
		case CollisionCode::XHitBoundary:
			left->m_velocity = left->m_velocity.reflect_x();
			break;
		case CollisionCode::YHitBoundary:
		case CollisionCode::YHitInside:
			left->m_velocity = left->m_velocity.reflect_y();
			break;
		case CollisionCode::None:
		default:
			break;
		}
	}

	__forceinline CollisionCode _rigidBody::is_collision(const _rigidBody* left, const _rigidBody* right) noexcept
	{
		// See also: https://www.acmicpc.net/problem/1002

	    const auto diff = left->get_position() - right->get_position();
	    const auto dist = std::sqrtf(std::powf(diff.get_x(), 2) + std::powf(diff.get_y(), 2));

	    // @todo: bug, if two object intersect, their collisions are done indefinitely.
	    // identical
	    if(left->m_position == right->get_position())
	    {
		    return CollisionCode::Identical;
	    }

		// @todo: case for negative value is needed.
	    const auto hitbox_diff = left->m_hitbox - right->m_hitbox;
	    // @note: using one hitbox size due to winapi middle point is actually top left.
	    //const auto hitbox_sum = m_hitbox + object.m_hitbox;
	    const auto& hitbox_sum = left->m_hitbox;

	    // Too far
	    if(hitbox_diff.get_x() > dist || hitbox_sum.get_x() < dist ||
	        hitbox_diff.get_y() > dist || hitbox_sum.get_y() < dist)
	    {
		    return CollisionCode::None;
	    }

	    // X Collision, meet each other in radius.
	    if(hitbox_diff.get_x() - dist < Math::epsilon || hitbox_sum.get_x() - dist < Math::epsilon)
	    {
		    return CollisionCode::XHitBoundary;
	    }
	    // X Collision, meet each other their inside.
	    if (hitbox_diff.get_x() < dist && dist < hitbox_sum.get_x())
	    {
	        return CollisionCode::XHitInside;
	    }

	    if (hitbox_diff.get_y() - dist < Math::epsilon || hitbox_sum.get_y() - dist < Math::epsilon)
	    {
			return CollisionCode::YHitBoundary;   
	    }
	    if (hitbox_diff.get_y() < dist && dist < hitbox_sum.get_y())
	    {
		    return CollisionCode::YHitInside;
	    }

	    return CollisionCode::None;
	}

	inline void _rigidBody::move(_rigidBody& object)
	{
		object += object.m_velocity * Fortress::DeltaTime::get_deltaTime();
	}
}
#endif // RIGIDBODY_HPP
