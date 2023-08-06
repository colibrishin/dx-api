#pragma once
#ifndef ENERGYBALLPROJECTILE_HPP
#define ENERGYBALLPROJECTILE_HPP

#include "GifWrapper.hpp"
#include "projectile.hpp"
#include "math.h"  // NOLINT(modernize-deprecated-headers)
#include "resourceManager.hpp"
#include "Texture.hpp"

namespace Fortress::Object
{
	class EnergyBallProjectile final : public ObjectBase::projectile
	{
	public:
		EnergyBallProjectile(const ObjectBase::character* shooter) : projectile(
			shooter,
			L"Energy Ball Projectile",
			L"secwind",
			{}, 
			Math::identity,
			5.0f,
			projectile_speed_getter(L"secwind", L"main"), 
			{}, 
			10.0f,
			30,
			1)
		{
			EnergyBallProjectile::initialize();
		}

		EnergyBallProjectile& operator=(const EnergyBallProjectile& other) = default;
		EnergyBallProjectile& operator=(EnergyBallProjectile&& other) = default;
		EnergyBallProjectile(const EnergyBallProjectile& other) = default;
		EnergyBallProjectile(EnergyBallProjectile&& other) = default;
		~EnergyBallProjectile() override = default;

		void initialize() override;
		virtual void play_hit_sound() override;
		virtual void play_fire_sound() override;
	};

	inline void EnergyBallProjectile::initialize()
	{
		projectile::initialize();
	}

	inline void EnergyBallProjectile::play_hit_sound()
	{
		m_sound_pack.get_sound(L"main-explosion").lock()->play(false);
	}

	inline void EnergyBallProjectile::play_fire_sound()
	{
		m_sound_pack.get_sound(L"main-fire").lock()->play(false);
	}
}
#endif // ENERGYBALLPROJECTILE_HPP
