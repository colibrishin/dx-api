#pragma once
#ifndef CHARACTER_HPP
#define CHARACTER_HPP

#include "CharacterController.hpp"
#include "rigidBody.hpp"
#include "common.h"
#include "sceneManager.hpp"
#include "Texture.hpp"
#include "ProjectileTimer.hpp"

// forward declaration for avoiding circular reference
namespace Fortress
{
	namespace Object
	{
		enum class GroundState;
		class Ground;
	}
}

namespace Fortress::ObjectBase
{
	class projectile;

	constexpr float character_full_hp = 100.0f;
	constexpr float character_full_mp = 1000.0f;
	constexpr float character_max_charge = 250.0f;

	class character : public Abstract::rigidBody, public Controller::CharacterController
	{
	public:
		character() = delete;
		character& operator=(const character& other) = default;
		character& operator=(character&& other) = default;
		character(const character& other) = default;
		character(character&& other) = default;
		~character() override = default;

		void initialize() override;
		void update() override;
		void render() override;
		void prerender() override;

		void hit(const std::weak_ptr<projectile>& p, const Math::Vector2& hit_point);

		const std::wstring& get_short_name() const;

		void on_collision(const CollisionCode& collision, const Math::Vector2& hit_vector, const std::weak_ptr<Abstract::rigidBody>& other) override;
		void on_nocollison() override;

		float get_armor() const;
	private:
		bool m_bGrounded;

		const std::wstring m_short_name;
		float m_armor;

		float get_damage_pen_dist(const std::weak_ptr<projectile>& p, const Math::Vector2& hit_point) const;

		void move() override;
		void fire() override;
		void move_left() override;
		void move_right() override;
		void stop() override;

		void render_hp_bar(const Math::Vector2& position);

		void ground_walk(const CollisionCode& collision, const std::weak_ptr<Object::Ground>& ptr_ground);
		void ground_gravity(const std::weak_ptr<Object::Ground>& ptr_ground);
		void ground_pitching(const std::weak_ptr<Object::Ground>& ptr_ground);

		Math::Vector2 get_next_velocity(const Math::Vector2& local_position_bottom,
		                                const std::weak_ptr<Object::Ground>& ground_ptr) const;

		std::weak_ptr<projectile> initialize_projectile(const Math::Vector2& angle, const float charged);

		ProjectileTimer m_multi_projectile_timer;

	protected:
		character(
			const std::wstring& name,
			const std::wstring& short_name,
			const Math::Vector2 offset,
			const Math::Vector2 position,
			const Math::Vector2 velocity,
			const float mass,
			const Math::Vector2& speed,
			const Math::Vector2& acceleration,
			const int hp,
			const int mp,
			const float armor);
	};
}

#endif
