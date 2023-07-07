#pragma once
#ifndef RIGIDBODY_HPP
#define RIGIDBODY_HPP

#include <vector>
#include <string>
#include "deltatime.hpp"
#include "math.h"
#include "debug.hpp"
#include "ground.hpp"
#include "object.hpp"
#include "winapihandles.hpp"

namespace ObjectInternal
{
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
		__forceinline static void move(_rigidBody* object);
		inline static std::vector<_rigidBody*> _known_rigid_bodies = {};

	protected:
		_rigidBody(const std::wstring& name, Math::Vector2 position, Math::Vector2 hitbox, Math::Vector2 velocity,
		           float speed,
		           float acceleration);
		_rigidBody(const _baseObject& obj);
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

	inline _rigidBody::_rigidBody(const _baseObject& obj) :
		_baseObject(obj),
		m_velocity({0.0f, 0.0f}),
		m_speed(0.0f),
	    m_acceleration(0.0f),
	    m_bActive(true),
	    m_bGrounded(false),
	    m_curr_speed(0.0f),
	    m_gravity_speed(0.0f),
	    m_gravity_acceleration(Math::G_ACC)
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
			// @todo: if the scene moves like "camera" then oob definition can be different with assumption.
			// @todo: if the character is oob in y-axis wise, then consider it as dead.
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
		// ground check
		for(const auto ground : Object::ground::_known_grounds)
		{
			if(!ground->m_bActive)
			{
				continue;
			}

			const Math::Vector2 diff = Math::Vector2{0.0f, ground->get_y()} - obj->m_position;
			
			_baseObject ground_base_class = _baseObject(*ground);
			CollisionCode code = is_collision(obj, &ground_base_class);

			const float y_diff = std::floorf(diff.get_y() - obj->m_hitbox.get_y());

			if(y_diff < 1.0f && 0 <= y_diff && 
				(static_cast<unsigned int>(code) & 
				static_cast<unsigned int>(CollisionCode::Bottom)))
			{
				Fortress::Debug::Log(L"Object hit ground");
				obj->m_bGrounded = true;
				obj->m_gravity_speed = 0.0f;
				return;
			}
		}

		// free-falling
		const Math::Vector2 diff = obj->m_position - Math::Vector2{0.0f, static_cast<float>(WinAPIHandles::get_actual_max_y())};

		if(std::floorf(diff.abs().get_y() - obj->m_hitbox.get_y()) == 0)
		{
			Fortress::Debug::Log(L"Object grounded");
			obj->m_bGrounded = true;
			obj->m_gravity_speed = 0.0f;
		}
		else
		{
			Fortress::Debug::Log(L"Object is floating");
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

		if(object->m_bGrounded && object->m_velocity.get_y() > Math::epsilon)
		{
			Fortress::Debug::Log(L"Object has tried to go under ground.");
			return;
		}

		//@todo: friction.
		object->m_curr_speed += object->m_acceleration * Fortress::DeltaTime::get_deltaTime() * 0.5f;
		*object += object->m_velocity * object->m_curr_speed * Fortress::DeltaTime::get_deltaTime();
		object->m_curr_speed += object->m_acceleration * Fortress::DeltaTime::get_deltaTime() * 0.5f;
	}
}
#endif // RIGIDBODY_HPP