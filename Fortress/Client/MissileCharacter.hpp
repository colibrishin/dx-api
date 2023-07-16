#pragma once
#ifndef CANNONCHARACTER_HPP
#define CANNONCHARACTER_HPP
#include "character.hpp"
#include "GifWrapper.hpp"
#include "math.h"
#include "MissileProjectile.hpp"
#include "resourceManager.hpp"

namespace Fortress::Object
{
	class MissileCharacter final : public ObjectBase::character
	{
	public:
		MissileCharacter(
			const std::wstring& name, 
			const Math::Vector2& position, 
			const Math::Vector2& orientation)
			: character(
				name,
				L"missile",
				position,
				orientation,
				{0.0f, 0.0f},
				500.0f,
				0.0f,
				ObjectBase::character_full_hp,
				ObjectBase::character_full_mp)
		{
			initialize();
		}

		~MissileCharacter() override
		{
			character::~character();
		}

		void initialize() override
		{
			set_sprite_offset(L"fire", L"right", {0, 10.0f});
			set_sprite_offset(L"fire", L"left", {0, 10.0f});
			set_sprite_offset(L"charging", L"right", {0, 10.0f});
			set_sprite_offset(L"charging", L"left", {0, 10.0f});

			m_base_projectile = std::make_shared<MissileProjectile>();

			character::initialize();
		}

		void shoot() override;
	private:
		std::shared_ptr<MissileProjectile> m_base_projectile;
	};
}

namespace Fortress::Object
{
	inline void MissileCharacter::shoot()
	{
		float charged = get_charged_power();

		if(charged < 0)
		{
			charged = 10.0f;
		}

		character::shoot();

		m_base_projectile->fire(
			get_offset() == Math::left ? get_top_left() : get_top_right(), 
			get_offset(), 
			charged);
	}
}

#endif // CANNONCHARACTER_HPP
