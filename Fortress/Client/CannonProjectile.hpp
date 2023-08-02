#pragma once
#ifndef CANNONPROJECTILE_HPP
#define CANNONPROJECTILE_HPP

#include "GifWrapper.hpp"
#include "projectile.hpp"
#include "math.h"  // NOLINT(modernize-deprecated-headers)
#include "resourceManager.hpp"
#include "Texture.hpp"

namespace Fortress::Object
{
	class CannonProjectile final : public ObjectBase::projectile
	{
	public:
		CannonProjectile(const ObjectBase::character* shooter) : projectile(
			shooter,
			L"Cannon Projectile",
			L"cannon",
			{}, 
			Math::identity,
			5.0f,
			projectile_speed_getter(L"cannon", L"main"), 
			{}, 
			10.0f,
			50,
			1)
		{
			CannonProjectile::initialize();
		}

		CannonProjectile& operator=(const CannonProjectile& other) = default;
		CannonProjectile& operator=(CannonProjectile&& other) = default;
		CannonProjectile(const CannonProjectile& other) = default;
		CannonProjectile(CannonProjectile&& other) = default;
		~CannonProjectile() override = default;

		void initialize() override;
		virtual void play_hit_sound() override;
		virtual void play_fire_sound() override;
	};

	inline void CannonProjectile::initialize()
	{
		projectile::initialize();
	}

	inline void CannonProjectile::play_hit_sound()
	{
		m_sound_pack.get_sound(L"main-explosion").lock()->play(false);
	}

	inline void CannonProjectile::play_fire_sound()
	{
		m_sound_pack.get_sound(L"main-fire").lock()->play(false);
	}
}
#endif // CANNONPROJECTILE_HPP
