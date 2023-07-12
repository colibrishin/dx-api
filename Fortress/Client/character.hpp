#pragma once
#ifndef CHARACTER_HPP
#define CHARACTER_HPP

#include "rigidBody.hpp"
#include "sceneManager.hpp"

namespace Fortress::ObjectBase
{
	class projectile;

	constexpr float character_full_hp = 100.0f;
	constexpr float character_full_mp = 100.0f;

	class character : public Abstract::rigidBody
	{
	public:
		character() = delete;
		character& operator=(const character& other) = delete;
		character& operator=(character&& other) = delete;

		void hit(const projectile* p);
		void update() override;
		virtual void firing();
		virtual void shoot();
		float get_charged_power() const;
		virtual void move() override;
		virtual void on_collision(rigidBody* other) override;

		float get_hp_percentage() const;
		float get_mp_percentage() const;
		~character() override;

	private:
		float m_hp;
		float m_mp;
		float m_power;
		bool m_bGrounded;

	protected:
		character(
			const std::wstring& name,
			const Math::Vector2 position,
			const Math::Vector2 velocity,
			const Math::Vector2 WH,
			const float speed,
			const float acceleration,
			const int hp,
			const int mp)
			: rigidBody(name, position, WH, velocity, speed, acceleration, true, true),
			  m_hp(hp),
			  m_mp(mp),
		      m_power(0.0f),
		      m_bGrounded(false)
		{
			character::initialize();
		}

		character(const character& other);
	};
}

#endif
