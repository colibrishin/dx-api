#pragma once
#ifndef NUTSHELLPROJECTILE_HPP
#define NUTSHELLPROJECTILE_HPP

#include "GifWrapper.hpp"
#include "projectile.hpp"
#include "math.h"  // NOLINT(modernize-deprecated-headers)
#include "resourceManager.hpp"
#include "Texture.hpp"

namespace Fortress::Object
{
	class NutShellProjectile final : public ObjectBase::projectile
	{
	public:
		NutShellProjectile(const ObjectBase::character* shooter) : projectile(
			shooter,
			L"Nutshell Projectile",
			shooter->get_short_name(),
			{}, 
			Math::identity,
			5.0f,
			projectile_speed_getter(shooter->get_short_name(), L"main"), 
			{}, 
			0.0f,
			1.0f,
			1,
			1)
		{
			NutShellProjectile::initialize();
		}

		NutShellProjectile& operator=(const NutShellProjectile& other) = default;
		NutShellProjectile& operator=(NutShellProjectile&& other) = default;
		NutShellProjectile(const NutShellProjectile& other) = default;
		NutShellProjectile(NutShellProjectile&& other) = default;
		~NutShellProjectile() override = default;

		void initialize() override;
		virtual void play_hit_sound() override;
		virtual void play_fire_sound() override;
	};

	inline void NutShellProjectile::initialize()
	{
		projectile::initialize();
	}

	inline void NutShellProjectile::play_hit_sound()
	{
		m_sound_pack.get_sound(L"main-explosion").lock()->play(false);
	}

	inline void NutShellProjectile::play_fire_sound()
	{
		m_sound_pack.get_sound(L"main-fire").lock()->play(false);
	}
}
#endif // NUTSHELLPROJECTILE_HPP
