#pragma once
#ifndef CHARACTER_H
#define CHARACTER_H

#include "object.h"
#include "rigidbody.hpp"

namespace Object
{
	enum class CharacterType
	{
		CANNON = 0,
	};

	constexpr int character_full_hp = 100;
	constexpr int character_full_mp = 100;

	class character final : public ObjectInternal::_rigidBody
	{
	public:
		character() :
		_rigidBody(L"", {0, 0}, {0, 0}, {0, 0}, 0, 0),
		m_hp(0),
		m_mp(0),
		m_type(CharacterType::CANNON) {}

		// copying is intended for preventing nullptr (use-after-free).
		character(
			const std::wstring& name,
			const Math::Vector2 position, 
			const Math::Vector2 velocity,
			const Math::Vector2 WH,
			const float speed,
			const float acceleration,
			const int hp,
			const int mp,
			const CharacterType type)
		: _rigidBody(name, position, WH, velocity, speed, acceleration), m_hp(hp), m_mp(mp), m_type(type) {}

		static void update();
		__forceinline float get_hp_percentage() const
		{
			return m_hp / character_full_hp;
		}
		__forceinline float get_mp_percentage() const
		{
			return m_mp / character_full_mp;
		}

	private:
		int m_hp;
		int m_mp;
		CharacterType m_type;
		void shoot();
	};

	inline void character::update()
	{
		_rigidBody::update();
	}
}

#endif
