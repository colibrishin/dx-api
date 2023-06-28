#pragma once
#ifndef CHARACTER_H
#define CHARACTER_H
#include "object.hpp"

namespace Object
{
	enum CharacterType
	{
		CANNON = 0,
	};

	class Character : ObjectInternal::_baseObject
	{
	public:
		Character(const float x, const float y, const int hp, const int mp, const CharacterType type)
		: _baseObject(x, y), m_hp(hp), m_mp(mp), m_type(type) {}
		
	private:
		int m_hp;
		int m_mp;
		CharacterType m_type;
	};
}

#endif