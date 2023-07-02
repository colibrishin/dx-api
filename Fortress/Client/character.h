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

	enum class CollisionCode
	{
		None = 0,
		Identical,
		XHitBoundary,
		XHitInside,
		YHitBoundary,
		YHitInside
	};

	class character final : public ObjectInternal::_baseObject
	{
	public:
		character() :
		_baseObject({0, 0}), m_hp(0), m_mp(0), m_type(CharacterType::CANNON), m_velocity(0, 0), m_hitbox(0, 0) {};

		// copying is intended for preventing nullptr (use-after-free).
		character(
			const Math::Vector2 position, 
			const Math::Vector2 velocity,
			const Math::Vector2 WH,
			const int hp, 
			const int mp,
			const CharacterType type)
		: _baseObject(position), m_velocity(velocity), m_hitbox(WH), m_hp(hp), m_mp(mp), m_type(type) {}

		CollisionCode is_collision(const character& object) const;

		Math::Vector2 m_velocity;
		Math::Vector2 m_hitbox;
	private:
		int m_hp;
		int m_mp;
		CharacterType m_type;
	};
}

#endif