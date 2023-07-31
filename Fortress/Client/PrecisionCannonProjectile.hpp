#pragma once
#ifndef PRECISIONCANNONPROJECTILE_HPP
#define PRECISIONCANNONPROJECTILE_HPP

#include "GifWrapper.hpp"
#include "projectile.hpp"
#include "math.h"  // NOLINT(modernize-deprecated-headers)
#include "resourceManager.hpp"
#include "Texture.hpp"

namespace Fortress::Object
{
	class PrecisionCannonProjectile final : public ObjectBase::projectile
	{
	public:
		PrecisionCannonProjectile(const ObjectBase::character* shooter) : projectile(
			shooter,
			L"Precision Cannon Projectile",
			L"cannon",
			{}, 
			Math::identity,
			5.0f,
			{200.0f, 1.0f}, 
			{}, 
			30.0f,
			10,
			1)
		{
			PrecisionCannonProjectile::initialize();
		}

		PrecisionCannonProjectile& operator=(const PrecisionCannonProjectile& other) = default;
		PrecisionCannonProjectile& operator=(PrecisionCannonProjectile&& other) = default;
		PrecisionCannonProjectile(const PrecisionCannonProjectile& other) = default;
		PrecisionCannonProjectile(PrecisionCannonProjectile&& other) = default;
		~PrecisionCannonProjectile() override = default;

		void initialize() override;
		virtual void play_hit_sound() override;
		virtual void play_fire_sound() override;
	};

	inline void PrecisionCannonProjectile::initialize()
	{
		projectile::initialize();
	}

	inline void PrecisionCannonProjectile::play_hit_sound()
	{
		m_sound_pack.get_sound(L"sub-explosion").lock()->play(false);
	}

	inline void PrecisionCannonProjectile::play_fire_sound()
	{
		m_sound_pack.get_sound(L"sub-fire").lock()->play(false);
	}
}
#endif // PRECISIONCANNONPROJECTILE_HPP
