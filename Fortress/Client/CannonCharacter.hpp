#pragma once
#ifndef CANNONCHARACTER_HPP
#define CANNONCHARACTER_HPP
#include "CannonProjectile.hpp"
#include "character.hpp"
#include "math.h"
#include "PrecisionCannonProjectile.hpp"

namespace Fortress::Object
{
	class CannonCharacter final : public ObjectBase::character
	{
	public:
		CannonCharacter(
			int player_id,
			const std::wstring& name, 
			const Math::Vector2& position, 
			const Math::Vector2& orientation)
			:character(
				player_id,
				name,
				L"cannon",
				orientation,
				position,
				{0.0f, 0.0f},
				1.0f,
				20.0f,
				0.0f,
				ObjectBase::character_full_hp,
				ObjectBase::character_full_mp,
				std::make_shared<CannonProjectile>(this),
				std::make_shared<PrecisionCannonProjectile>(this))
		{
			initialize();
		}
		CannonCharacter& operator=(const CannonCharacter& other) = default;
		CannonCharacter& operator=(CannonCharacter&& other) = default;
		CannonCharacter(const CannonCharacter& other) = default;
		CannonCharacter(CannonCharacter&& other) = default;
		~CannonCharacter() override = default;

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
	inline void CannonCharacter::shoot()
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

#endif // CANNONCHARACTER_HPP
