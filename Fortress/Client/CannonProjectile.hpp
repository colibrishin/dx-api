#pragma once
#ifndef CANNONPROJECTILE_HPP
#define CANNONPROJECTILE_HPP

#include "ClientProjectile.hpp"
#include "CharacterProperties.hpp"
#include "../Common/projectile.hpp"

namespace Fortress::Object
{
	class CannonProjectile final : public Network::Client::Object::ClientProjectile
	{
	public:
		CannonProjectile(const unsigned int id, const ObjectBase::character* shooter) : ClientProjectile(
			id,
			shooter,
			L"Cannon Projectile",
			L"cannon",
			{}, 
			Math::identity,
			5.0f,
			Property::projectile_speed_getter(L"cannon", L"main"), 
			{}, 
			Property::projectile_damage_getter(L"cannon", L"main"),
			Property::projectile_radius_getter(L"cannon", L"main"),
			1,
			1,
			Property::projectile_pen_rate_getter(L"cannon", L"main"))
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
		eProjectileType get_type() const override;
	};

	inline void CannonProjectile::initialize()
	{
		ClientProjectile::initialize();
	}

	inline void CannonProjectile::play_hit_sound()
	{
		m_sound_pack.get_sound(L"main-explosion").lock()->play(false);
	}

	inline void CannonProjectile::play_fire_sound()
	{
		m_sound_pack.get_sound(L"main-fire").lock()->play(false);
	}

	inline eProjectileType CannonProjectile::get_type() const
	{
		return eProjectileType::Main;
	}
}
#endif // CANNONPROJECTILE_HPP
