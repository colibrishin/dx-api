#pragma once
#ifndef RIGIDBODY_HPP
#define RIGIDBODY_HPP

#include <vector>
#include <string>
#include "deltatime.hpp"
#include "math.h"
#include "debug.hpp"
#include "Ground.hpp"
#include "object.hpp"
#include "winapihandles.hpp"

namespace Fortress::Abstract
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

	class rigidBody : public object
	{
	public:
		Math::Vector2 m_velocity;
		float m_speed;
		float m_acceleration;
		bool m_bActive;
		bool m_bGrounded;

		// rigidBody should be used with inheritance
		rigidBody() = delete;
		rigidBody& operator=(const rigidBody& other) = delete;
		rigidBody& operator=(rigidBody&& other) = delete;

		__forceinline void initialize() override;
		__forceinline static void update();
		__forceinline static void block_window_frame(rigidBody& target);

		__forceinline void move_down() override;
		__forceinline void move_left() override;
		__forceinline void move_right() override;
		__forceinline void move_up() override;
		__forceinline void stop();

		virtual bool is_movable()
		{
			return true;
		}
	private:
		float m_curr_speed;
		float m_gravity_speed;
		float m_gravity_acceleration;

		__forceinline static void apply_gravity(rigidBody* obj);
		__forceinline static void update_collision(rigidBody* left, rigidBody* right) noexcept;
		__forceinline static void move(rigidBody* object);

	protected:
		rigidBody(const std::wstring& name, Math::Vector2 position, Math::Vector2 hitbox, Math::Vector2 velocity,
		          float speed,
		          float acceleration);
		~rigidBody() override;
		rigidBody(const rigidBody& other);

		inline static std::vector<rigidBody*> _known_rigid_bodies = {};
		std::vector<object*> collision_lists;
	};
}

namespace Fortress::Abstract
{
	inline rigidBody::rigidBody(const std::wstring& name, const Math::Vector2 position, const Math::Vector2 hitbox,
	                            const Math::Vector2 velocity, const float speed, const float acceleration) :
		object(name, position, hitbox),
		m_velocity(velocity),
		m_speed(speed),
		m_acceleration(acceleration),
		m_bActive(true),
		m_bGrounded(false),
		m_curr_speed(0.0f),
		m_gravity_speed(0.0f),
		m_gravity_acceleration(Math::G_ACC)
	{
		rigidBody::initialize();
	}

	__forceinline void rigidBody::initialize()
	{
		object::initialize();
		_known_rigid_bodies.push_back(this);
	}

	__forceinline void rigidBody::update()
	{
		for (const auto left_r : _known_rigid_bodies)
		{
			for (const auto right_r : _known_rigid_bodies)
			{
				if (left_r == right_r || !left_r->m_bActive || !right_r->m_bActive)
				{
					continue;
				}

				// @todo: performance degrading
				left_r->collision_lists.clear();
				update_collision(left_r, right_r);
			}
		}

		for (const auto r : _known_rigid_bodies)
		{
			if (!r->m_bActive)
			{
				continue;
			}

			if (r->is_movable())
			{
				move(r);
			}

			apply_gravity(r);
		}
	}

	inline void rigidBody::block_window_frame(rigidBody& target)
	{
		const float x = target.m_hitbox.get_x() * 2;
		const float y = target.m_hitbox.get_y();

		if (target.get_x() > WinAPIHandles::get_window_width() ||
			target.get_y() > WinAPIHandles::get_actual_max_y())
		{
			// OOB
			// @todo: if the scene moves like "camera" then oob definition can be different with assumption.
			// @todo: if the character is oob in y-axis wise, then consider it as dead.
			target.m_position = {1.0f, 1.0f};
			return;
		}

		if (target.get_x() <= 0)
		{
			target.m_velocity = target.m_velocity.reflect_x();
			target.m_position += {1.0f, 0.0f};
		}
		else if (target.get_x() - (WinAPIHandles::get_window_width() - x) > Math::epsilon)
		{
			target.m_velocity = target.m_velocity.reflect_x();
			target.m_position -= {1.0f, 0.0f};
		}

		if (target.get_y() <= 0)
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

	inline rigidBody::~rigidBody()
	{
		if (!_known_rigid_bodies.empty())
		{
			_known_rigid_bodies.erase(
				std::remove_if(
					_known_rigid_bodies.begin(),
					_known_rigid_bodies.end(),
					[this](const rigidBody* r)
					{
						return r == this;
					}),
				_known_rigid_bodies.end());
		}

		object::~object();
	}

	inline rigidBody::rigidBody(const rigidBody& other) :
		object(other),
		m_velocity(other.m_velocity),
		m_speed(other.m_speed),
		m_acceleration(other.m_acceleration),
		m_bActive(other.m_bActive),
		m_bGrounded(other.m_bGrounded),
		m_curr_speed(other.m_curr_speed),
		m_gravity_speed(other.m_gravity_speed),
		m_gravity_acceleration(other.m_gravity_acceleration)
	{
	}

	inline void rigidBody::move_down()
	{
		m_velocity = {0, 1.0f};
	}

	inline void rigidBody::move_left()
	{
		m_velocity = {-1.0f, 0};
	}

	inline void rigidBody::move_right()
	{
		m_velocity = {1.0f, 0};
	}

	inline void rigidBody::move_up()
	{
		m_velocity = {0, -1.0f};
	}

	inline void rigidBody::stop()
	{
		m_velocity = {0, 0};
	}

	__forceinline static CollisionCode is_collision(const object* left, const object* right) noexcept
	{
		// See also: https://www.acmicpc.net/problem/1002

		// force to pivoting from top left to mid point.
		const auto left_mid = Math::Vector2{
			left->get_x() + left->m_hitbox.get_x() / 2, left->get_y() + left->m_hitbox.get_y() / 2
		};
		const auto right_mid = Math::Vector2{
			right->get_x() + right->m_hitbox.get_x() / 2, right->get_y() + right->m_hitbox.get_y() / 2
		};
		const auto diff = left_mid - right_mid;

		// identical
		if (left_mid == right_mid)
		{
			return CollisionCode::Identical;
		}

		const auto hitbox_diff = left->m_hitbox - right->m_hitbox;
		const auto& hitbox_sum = left->m_hitbox + right->m_hitbox;

		// Too far
		if (hitbox_diff.get_x() > diff.get_x() || hitbox_sum.get_x() < diff.get_x() ||
			hitbox_diff.get_y() > diff.get_y() || hitbox_sum.get_y() < diff.get_y())
		{
			return CollisionCode::None;
		}

		float x = 0;
		float y = 0;

		// X Collision, meet each other in radius or meet each other their inside.
		if (hitbox_diff.get_x() - diff.get_x() < Math::epsilon || hitbox_sum.get_x() - diff.get_x() < Math::epsilon ||
			hitbox_diff.get_x() < diff.get_x() && diff.get_x() < hitbox_sum.get_x())
		{
			x = diff.normalized().get_x();
		}

		if (hitbox_diff.get_y() - diff.get_y() < Math::epsilon || hitbox_sum.get_y() - diff.get_y() < Math::epsilon ||
			hitbox_diff.get_y() < diff.get_y() && diff.get_y() < hitbox_sum.get_y())
		{
			y = diff.normalized().get_y();
		}

		if (std::fabs(x) < Math::epsilon && std::fabs(y) < Math::epsilon)
		{
			Debug::Log(L"None");
			return CollisionCode::None;
		}

		// using epsilon instead of zero.
		if (x > 0 && std::fabs(y) < Math::epsilon)
		{
			Debug::Log(L"Left");
			return CollisionCode::Left;
		}
		if (x < 0 && std::fabs(y) < Math::epsilon)
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
		if (x > 0 && y < 0)
		{
			Debug::Log(L"BottomLeft");
			return CollisionCode::BottomLeft;
		}
		if (x < 0 && y > 0)
		{
			Debug::Log(L"TopRight");
			return CollisionCode::TopRight;
		}
		if (x < 0 && y < 0)
		{
			Debug::Log(L"BottomRight");
			return CollisionCode::BottomRight;
		}
		if (x > 0 && y > 0)
		{
			Debug::Log(L"TopLeft");
			return CollisionCode::TopLeft;
		}

		return CollisionCode::None;
	}

	inline void rigidBody::apply_gravity(rigidBody* obj)
	{
		// Ground check
		for (const auto ground : Object::Ground::_known_grounds)
		{
			if (!ground->m_bActive)
			{
				continue;
			}

			const Math::Vector2 diff = Math::Vector2{0.0f, ground->get_y()} - obj->m_position;

			const object* ground_base_class = ground;
			CollisionCode code = is_collision(obj, ground_base_class);

			const float y_diff = std::floorf(diff.get_y() - obj->m_hitbox.get_y());

			if (y_diff < 1.0f && 0 <= y_diff &&
				(static_cast<unsigned int>(code) &
					static_cast<unsigned int>(CollisionCode::Bottom)))
			{
				Debug::Log(L"Object hit Ground");
				obj->m_bGrounded = true;
				obj->m_gravity_speed = 0.0f;
				obj->collision_lists.push_back(ground);
				return;
			}
		}

		// free-falling
		const Math::Vector2 diff = obj->m_position - Math::Vector2{
			0.0f, static_cast<float>(WinAPIHandles::get_actual_max_y())
		};

		if (std::floorf(diff.abs().get_y() - obj->m_hitbox.get_y()) == 0)
		{
			Debug::Log(L"Object grounded");
			obj->m_bGrounded = true;
			obj->m_gravity_speed = 0.0f;
		}
		else
		{
			Debug::Log(L"Object is floating");
			obj->m_bGrounded = false;
		}

		if (!obj->m_bGrounded)
		{
			obj->m_gravity_speed += obj->m_gravity_acceleration * DeltaTime::get_deltaTime() * 0.5f;
			*obj += {0.0f, obj->m_gravity_speed * DeltaTime::get_deltaTime()};
			obj->m_gravity_speed += obj->m_gravity_acceleration * DeltaTime::get_deltaTime() * 0.5f;
		}
	}

	__forceinline void rigidBody::update_collision(rigidBody* left, rigidBody* right) noexcept
	{
		CollisionCode code = is_collision(left, right);

		if (code == CollisionCode::None)
		{
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

		left->collision_lists.push_back(right);
	}

	inline void rigidBody::move(rigidBody* object)
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

		if (object->m_bGrounded && object->m_velocity.get_y() > Math::epsilon)
		{
			Debug::Log(L"Object has tried to go under Ground.");
			return;
		}

		//@todo: friction.
		object->m_curr_speed += object->m_acceleration * DeltaTime::get_deltaTime() * 0.5f;
		*object += object->m_velocity * object->m_curr_speed * DeltaTime::get_deltaTime();
		object->m_curr_speed += object->m_acceleration * DeltaTime::get_deltaTime() * 0.5f;
	}
}

#endif // RIGIDBODY_HPP
