#pragma once
#ifndef PRECISIONCANNONPROJECTILE_HPP
#define PRECISIONCANNONPROJECTILE_HPP

#include "CharacterProperties.hpp"
#include "../Common/projectile.hpp"

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
			Property::projectile_speed_getter(L"cannon", L"sub"), 
			{}, 
			Property::projectile_damage_getter(L"cannon", L"sub"),
			Property::projectile_radius_getter(L"cannon", L"sub"),
			1,
			1,
			Property::projectile_pen_rate_getter(L"cannon", L"sub"))
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
		eProjectileType get_type() const override;
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

	inline eProjectileType PrecisionCannonProjectile::get_type() const
	{
		return eProjectileType::Sub;
	}
}
#endif // PRECISIONCANNONPROJECTILE_HPP
