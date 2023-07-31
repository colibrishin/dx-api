#pragma once
#ifndef MISSILEPROJECTILE_HPP
#define MISSILEPROJECTILE_HPP

#include "GifWrapper.hpp"
#include "projectile.hpp"
#include "math.h"

namespace Fortress::Object
{
	class MissileProjectile final : public ObjectBase::projectile
	{
	public:
		MissileProjectile(const ObjectBase::character* shooter) : projectile(
			shooter,
			L"Precision Projectile",
			L"missile",
			{}, 
			Math::identity,
			5.0f,
			2.0f, 
			0.0f, 
			10.0f,
			30,
			2)
		{
			MissileProjectile::initialize();
		}

		MissileProjectile& operator=(const MissileProjectile& other) = default;
		MissileProjectile& operator=(MissileProjectile&& other) = default;
		MissileProjectile(const MissileProjectile& other) = default;
		MissileProjectile(MissileProjectile&& other) = default;
		~MissileProjectile() override = default;

		void initialize() override;
		virtual void play_hit_sound() override;
		virtual void play_fire_sound() override;
	};

	inline void MissileProjectile::initialize()
	{
		projectile::initialize();
	}

	inline void MissileProjectile::play_hit_sound()
	{
		m_sound_pack.get_sound(L"main-explosion").lock()->play(false);
	}

	inline void MissileProjectile::play_fire_sound()
	{
		m_sound_pack.get_sound(L"main-fire").lock()->play(false);
	}
}
#endif // CANNONPROJECTILE_HPP
