#pragma once
#ifndef MULTIENERGYBALLPROJECTILE_HPP
#define MULTIENERGYBALLPROJECTILE_HPP

#include "GifWrapper.h"
#include "projectile.hpp"
#include "math.h"  // NOLINT(modernize-deprecated-headers)
#include "resourceManager.hpp"
#include "Texture.hpp"

namespace Fortress::Object
{
	class MultiEnergyBallProjectile final : public ObjectBase::projectile
	{
	public:
		MultiEnergyBallProjectile(const ObjectBase::character* shooter) : projectile(
			shooter,
			L"Multi Energy Ball Projectile",
			L"secwind",
			{}, 
			Math::identity,
			5.0f,
			projectile_speed_getter(L"secwind", L"sub"), 
			{}, 
			10.0f,
			30,
			1,
			3,
			0.5f)
		{
			MultiEnergyBallProjectile::initialize();
		}

		MultiEnergyBallProjectile& operator=(const MultiEnergyBallProjectile& other) = default;
		MultiEnergyBallProjectile& operator=(MultiEnergyBallProjectile&& other) = default;
		MultiEnergyBallProjectile(const MultiEnergyBallProjectile& other) = default;
		MultiEnergyBallProjectile(MultiEnergyBallProjectile&& other) = default;
		~MultiEnergyBallProjectile() override = default;

		void initialize() override;
		virtual void play_hit_sound() override;
		virtual void play_fire_sound() override;
	};

	inline void MultiEnergyBallProjectile::initialize()
	{
		projectile::initialize();
	}

	inline void MultiEnergyBallProjectile::play_hit_sound()
	{
		m_sound_pack.get_sound(L"sub-explosion").lock()->play(false);
	}

	inline void MultiEnergyBallProjectile::play_fire_sound()
	{
		m_sound_pack.get_sound(L"sub-fire").lock()->play(false);
	}
}
#endif // MULTIENERGYBALLPROJECTILE_HPP
