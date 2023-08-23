#pragma once
#ifndef PROJECTILE_HPP
#define PROJECTILE_HPP

#include "character.hpp"
#include "ProjectileController.hpp"
#include "rigidBody.hpp"

namespace Fortress::ObjectBase
{
	constexpr float default_explosion_radius = 10.0f;

	class projectile : public Abstract::rigidBody, public Controller::ProjectileController
	{
	public:
		projectile() = delete;
		projectile& operator=(const projectile& other) = default;
		projectile& operator=(projectile&& other) = default;
		projectile(const projectile& other) = default;
		projectile(projectile&& other) = default;
		~projectile() override = default;

		static Math::Vector2 projectile_speed_getter(const std::wstring& short_name, const std::wstring& type);

		void initialize() override;
		void update() override;
		void render() override;
		void prerender() override;

		void on_collision(
			const CollisionCode& collision, 
			const GlobalPosition& collision_point,
			const std::weak_ptr<rigidBody>& other) override;

		virtual void fire(const Math::Vector2& position, const Math::Vector2& velocity, const float charged);

		float get_radius() const;
		float get_damage() const;
		float get_penetration_rate() const;

	protected:
		projectile(
			const character* shooter,
			const std::wstring& name,
			const std::wstring& short_name,
			const Math::Vector2& position,
			const Math::Vector2& velocity,
			const float mass,
			const Math::Vector2& speed,
			const Math::Vector2& acceleration,
			const float damage,
			const float radius,
			const int hit_count,
			const int fire_count,
			const float armor_penetration);

		void move() override;
		void fire() override;
		void flying() override;
		void hit() override;
		void destroyed() override;

	private:
		float m_damage;
		float m_radius;
		float m_armor_penetration;

		Math::Vector2 m_wind_acceleration;
	};
}
#endif // PROJECTILE_HPP
