#pragma once
#ifndef SECNWINDCHARACTER_HPP
#define SECNWINDCHARACTER_HPP
#include "character.hpp"
#include "EnergyBallProjectile.hpp"
#include "GuidedMissileProjectile.hpp"
#include "math.h"
#include "MissileProjectile.hpp"

namespace Fortress::Object
{
	class SecwindCharacter final : public ObjectBase::character
	{
	public:
		SecwindCharacter(
			int player_id,
			const std::wstring& name, 
			const Math::Vector2& position, 
			const Math::Vector2& orientation)
			:
			character(
				player_id,
				name,
				L"secwind",
				orientation,
				position,
				{0.0f, 0.0f},
				1.0f,
				{25.0f, 1.0f},
				{},
				ObjectBase::character_full_hp,
				ObjectBase::character_full_mp,
				std::make_shared<EnergyBallProjectile>(this),
				std::make_shared<EnergyBallProjectile>(this))
		{
			initialize();
		}
		SecwindCharacter& operator=(const SecwindCharacter& other) = default;
		SecwindCharacter& operator=(SecwindCharacter&& other) = default;
		SecwindCharacter(const SecwindCharacter& other) = default;
		SecwindCharacter(SecwindCharacter&& other) = default;
		~SecwindCharacter() override = default;

		void initialize() override
		{
			character::initialize();
		}

		void shoot() override;
	};
}

namespace Fortress::Object
{
	inline void SecwindCharacter::shoot()
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
			angle = {-cosf(get_movement_pitch_radian()), -sinf(get_movement_pitch_radian())};
		}
		else
		{
			angle = {cosf(get_movement_pitch_radian()), sinf(get_movement_pitch_radian())};
		}

		const auto projectile = get_current_projectile().lock();

		const auto forward = Math::Vector2{get_offset().get_x(), -1} * projectile->m_hitbox.get_x();
		const auto forward_rotation = forward.rotate(get_movement_pitch_radian());

		// @todo: multiple projectile handling

		get_current_projectile().lock()->fire(
			(get_offset() == Math::left ? get_top_left() : get_top_right()) + forward_rotation,
			angle, 
			charged);
	}
}

#endif // SECNWINDCHARACTER_HPP
