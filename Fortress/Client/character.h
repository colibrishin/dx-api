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

	class character final : public ObjectInternal::_rigidBody
	{
	public:
		character() :
		_rigidBody({0, 0}, {0, 0}, {0, 0}, 0, 0),
		m_hp(0),
		m_mp(0),
		m_type(CharacterType::CANNON) {}

		// copying is intended for preventing nullptr (use-after-free).
		character(
			const Math::Vector2 position, 
			const Math::Vector2 velocity,
			const Math::Vector2 WH,
			const float speed,
			const float acceleration,
			const int hp, 
			const int mp,
			const CharacterType type)
		: _rigidBody(position, WH, velocity, speed, acceleration), m_hp(hp), m_mp(mp), m_type(type) {}

		static void update();

	private:
		int m_hp;
		int m_mp;
		CharacterType m_type;
	};

	inline void character::update()
	{
		_rigidBody::update();
	}
}

#endif