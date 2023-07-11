#pragma once
#ifndef GRAVITY_RIGIDBODY_HPP
#define GRAVITY_RIGIDBODY_HPP
#include "rigidBody.hpp"

namespace Fortress::Abstract
{
	class gravity_rigidBody : public rigidBody
	{
	public:
		gravity_rigidBody() = delete;
		gravity_rigidBody& operator=(const gravity_rigidBody& other) = default;
		gravity_rigidBody& operator=(gravity_rigidBody&& other) = default;

		virtual void initialize() override;
		virtual void update() override;
		virtual void render() override;

	protected:
		gravity_rigidBody(
			const std::wstring& name, const Math::Vector2 position, const Math::Vector2 WH, Math::Vector2 velocity,
			const float speed, const float acceleration) :
		rigidBody(name, position, WH, velocity, speed, acceleration),
		m_gravity_speed(0.0f),
		m_gravity_acceleration(Math::G_ACC) {}
		virtual ~gravity_rigidBody() override = default;

	private:
		float m_gravity_speed;
		float m_gravity_acceleration;
	};

	inline void gravity_rigidBody::render()
	{
		rigidBody::render();
	}

	inline void gravity_rigidBody::update()
	{
		rigidBody::update();

		// free-falling
		const Math::Vector2 diff = m_position - Math::Vector2{
			0.0f, static_cast<float>(WinAPIHandles::get_actual_max_y())};

		// @todo: if object speed is too fast, then this condition would not work properly.
		if (std::floorf(diff.abs().get_y() - m_hitbox.get_y()) <= 0)
		{
			m_gravity_speed = 0.0f;
		}
		else
		{
			m_gravity_speed += m_gravity_acceleration * DeltaTime::get_deltaTime() * 0.5f;
			*this += {0.0f, m_gravity_speed * DeltaTime::get_deltaTime()};
			m_gravity_speed += m_gravity_acceleration * DeltaTime::get_deltaTime() * 0.5f;
		}
	}

	inline void gravity_rigidBody::initialize()
	{
		rigidBody::initialize();
	}
}
#endif // GRAVITY_RIGIDBODY_HPP
