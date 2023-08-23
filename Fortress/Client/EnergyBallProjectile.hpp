#pragma once
#ifndef ENERGYBALLPROJECTILE_HPP
#define ENERGYBALLPROJECTILE_HPP

#include "ClientProjectile.hpp"
#include "CharacterProperties.hpp"
#include "../Common/projectile.hpp"

namespace Fortress::Object
{
	class EnergyBallProjectile final : public Network::Client::Object::ClientProjectile
	{
	public:
		EnergyBallProjectile(const unsigned int id, const ObjectBase::character* shooter) : ClientProjectile(
			id,
			shooter,
			L"Energy Ball Projectile",
			L"secwind",
			{}, 
			Math::identity,
			5.0f,
			Property::projectile_speed_getter(L"secwind", L"main"), 
			{}, 
			Property::projectile_damage_getter(L"secwind", L"main"),
			Property::projectile_radius_getter(L"secwind", L"main"),
			1,
			1,
			Property::projectile_pen_rate_getter(L"secwind", L"main"))
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
		eProjectileType get_type() const override;
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

	inline eProjectileType EnergyBallProjectile::get_type() const
	{
		return eProjectileType::Main;
	}
}
#endif // ENERGYBALLPROJECTILE_HPP
