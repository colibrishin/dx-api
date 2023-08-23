#pragma once
#ifndef MULTIENERGYBALLPROJECTILE_HPP
#define MULTIENERGYBALLPROJECTILE_HPP

#include "ClientProjectile.hpp"
#include "CharacterProperties.hpp"
#include "../Common/projectile.hpp"

namespace Fortress::Object
{
	class MultiEnergyBallProjectile final : public Network::Client::Object::ClientProjectile
	{
	public:
		MultiEnergyBallProjectile(const unsigned int id, const ObjectBase::character* shooter) : ClientProjectile(
			id,
			shooter,
			L"Multi Energy Ball Projectile",
			L"secwind",
			{}, 
			Math::identity,
			5.0f,
			Property::projectile_speed_getter(L"secwind", L"sub"), 
			{}, 
			Property::projectile_damage_getter(L"secwind", L"sub"),
			Property::projectile_radius_getter(L"secwind", L"sub"),
			1,
			3,
			Property::projectile_pen_rate_getter(L"secwind", L"sub"))
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
		eProjectileType get_type() const override;
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

	inline eProjectileType MultiEnergyBallProjectile::get_type() const
	{
		return eProjectileType::Sub;
	}
}
#endif // MULTIENERGYBALLPROJECTILE_HPP
