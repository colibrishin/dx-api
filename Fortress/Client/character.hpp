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
		__forceinline static void update();

		__forceinline virtual void shoot()
		{
		};
		__forceinline bool is_movable() override;

		__forceinline float get_hp_percentage() const
		{
			return m_hp / static_cast<float>(character_full_hp);
		}

		__forceinline float get_mp_percentage() const
		{
			return m_mp / static_cast<float>(character_full_mp);
		}

		__forceinline ~character() override
		{
			rigidBody::~rigidBody();
		}

		character(
			const std::wstring& name,
			const Math::Vector2 position,
			const Math::Vector2 velocity,
			const Math::Vector2 WH,
			const float speed,
			const float acceleration,
			const int hp,
			const int mp)
			: rigidBody(name, position, WH, velocity, speed, acceleration),
			  m_hp(hp),
			  m_mp(mp)
		{
			character::initialize();
		}

	private:
		float m_hp;
		float m_mp;

	protected:
		character(const character& other);
	};

	__forceinline void character::update()
	{
		rigidBody::update();
	}

	__forceinline bool character::is_movable()
	{
		if (m_mp < Math::epsilon)
		{
			return false;
		}

		// @todo: going down also reduces the movement point.
		if (std::fabs(m_velocity.get_x()) > Math::epsilon ||
			std::fabs(m_velocity.get_y()) > Math::epsilon)
		{
			m_mp -= 20.0f * DeltaTime::get_deltaTime();
		}

		return true;
	}

	inline character::character(const character& other) :
		rigidBody(other), m_hp(other.m_hp), m_mp(other.m_mp)
	{
		// Calling initialization once in copy constructor is needed.
		// If not, deconstructor removes the pointer and nothing updates the list.
		character::initialize();
	}
}

#endif
