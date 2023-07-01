#pragma once
#ifndef CHARACTER_H
#define CHARACTER_H
#include "object.hpp"

namespace Object
{
	enum class CharacterType
	{
		CANNON = 0,
	};

	class character final : public ObjectInternal::_baseObject
	{
	public:
		character() : _baseObject({0, 0}), m_hp(0), m_mp(0), m_type(CharacterType::CANNON), m_velocity(0, 0) {};
		character(const Math::Vector2 position, const Math::Vector2 velocity, const int hp, const int mp, const CharacterType type)
		: _baseObject(position), m_hp(hp), m_mp(mp), m_velocity(velocity), m_type(type) {}

		Math::Vector2 m_velocity;
	private:
		int m_hp;
		int m_mp;
		CharacterType m_type;
	};
}

#endif