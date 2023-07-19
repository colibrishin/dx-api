#pragma once
#ifndef MISSILECHARACTER_HPP
#define MISSILECHARACTER_HPP
#include "character.hpp"
#include "GifWrapper.hpp"
#include "GuidedMissileProjectile.hpp"
#include "math.h"
#include "MissileProjectile.hpp"
#include "resourceManager.hpp"

namespace Fortress::Object
{
	class MissileCharacter final : public ObjectBase::character
	{
	public:
		MissileCharacter(
			int player_id,
			const std::wstring& name, 
			const Math::Vector2& position, 
			const Math::Vector2& orientation)
			:
			character(
				player_id,
				name,
				L"missile",
				orientation,
				position,
				{0.0f, 0.0f},
				1.0f,
				20.0f,
				0.0f,
				ObjectBase::character_full_hp,
				ObjectBase::character_full_mp,
				std::make_shared<MissileProjectile>(),
				std::make_shared<GuidedMissileProjectile>())
		{
			initialize();
		}
		MissileCharacter& operator=(const MissileCharacter& other) = default;
		MissileCharacter& operator=(MissileCharacter&& other) = default;
		MissileCharacter(const MissileCharacter& other) = default;
		MissileCharacter(MissileCharacter&& other) = default;
		~MissileCharacter() override = default;

		void initialize() override
		{
			set_sprite_offset(L"fire", L"right", {0, 10.0f});
			set_sprite_offset(L"fire", L"left", {0, 10.0f});
			set_sprite_offset(L"charging", L"right", {0, 10.0f});
			set_sprite_offset(L"charging", L"left", {0, 10.0f});

			character::initialize();
		}

		void shoot() override;
	};
}

namespace Fortress::Object
{
	inline void MissileCharacter::shoot()
	{
		float charged = get_charged_power();

		if(charged <= 0.0f)
		{
			charged = 10.0f;
		}

		character::shoot();

		Math::Vector2 angle{};

		if(get_offset() == Math::left)
		{
			angle = {-std::cosf(Math::to_radian(45.0f)), -std::cosf(Math::to_radian(45.0f))};
		}
		else
		{
			angle = {std::cosf(Math::to_radian(45.0f)), -std::cosf(Math::to_radian(45.0f))};
		}

		get_current_projectile().lock()->fire(
			get_offset() == Math::left ? get_top_left() : get_top_right(),
			angle, 
			charged);
	}
}

#endif // MISSILECHARACTER_HPP
