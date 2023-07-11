#pragma once
#ifndef PROJECTILE_HPP
#define PROJECTILE_HPP

#include "character.hpp"
#include "rigidBody.hpp"

namespace Fortress::ObjectBase
{
	constexpr float default_explosion_radius = 10.0f;

	class projectile : public Abstract::rigidBody
	{
	public:
		projectile() = delete;
		projectile& operator=(const projectile& other) = delete;
		projectile& operator=(projectile&& other) = delete;

		__forceinline void initialize() override;
		__forceinline virtual void update() override;
		virtual void on_collision(rigidBody* other) override;

		float get_damage() const;

	protected:
		projectile(
			const std::wstring& name,
			const Math::Vector2& position,
			const Math::Vector2& WH,
			const Math::Vector2& velocity,
			const float speed,
			const float acceleration,
			const float damage) :
			rigidBody(name, position, WH, velocity, speed, acceleration, true, true),
			m_damage(damage)
		{
			projectile::initialize();
		}

		__forceinline ~projectile() override
		{
			rigidBody::~rigidBody();
		}

	private:
		float m_damage;
	};

	__forceinline void projectile::update()
	{
	}
}
#endif // PROJECTILE_HPP
