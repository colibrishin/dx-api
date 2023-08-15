#pragma once
#ifndef GUIDEDMISSILEPROJECTILE_HPP
#define GUIDEDMISSILEPROJECTILE_HPP

#include "GifWrapper.h"
#include "projectile.hpp"
#include "math.h"
#include "scene.hpp"

namespace Fortress::Object
{
	class MissileCharacter;

	class GuidedMissileProjectile final : public ObjectBase::projectile
	{
	public:
		GuidedMissileProjectile(const ObjectBase::character* shooter) : projectile(
			shooter,
			L"Guided Precision Projectile",
			L"missile",
			{}, 
			Math::identity,
			5.0f,
			projectile_speed_getter(L"missile", L"sub"), 
			{}, 
			10.0f,
			10,
			1,
			1,
			0.7f),
			m_bLocked(false),
			m_bSoundPlayed(false)
		{
			GuidedMissileProjectile::initialize();
		}

		GuidedMissileProjectile& operator=(const GuidedMissileProjectile& other) = default;
		GuidedMissileProjectile& operator=(GuidedMissileProjectile&& other) = default;
		GuidedMissileProjectile(const GuidedMissileProjectile& other) = default;
		GuidedMissileProjectile(GuidedMissileProjectile&& other) = default;
		~GuidedMissileProjectile() override = default;

		void update() override;
		void initialize() override;
		virtual void play_hit_sound() override;
		virtual void play_fire_sound() override;
		void play_homming_sound();

	private:
		std::weak_ptr<ObjectBase::character> m_locked_target;
		bool m_bLocked;
		bool m_bSoundPlayed;

		void destroyed() override;
	};

	inline void GuidedMissileProjectile::update()
	{
		const auto x_velocity = m_velocity * Math::Vector2{1, 0};
		const auto detection_point = x_velocity.get_x() < 0 ? get_bottom_left() : get_bottom_right();

		if(const auto scene_ptr = Scene::SceneManager::get_active_scene().lock())
		{
			// @todo: if radius is big enough to contains shooter itself then missile is moving back and forth.
			const auto characters = scene_ptr->is_in_range<ObjectBase::character>(
				detection_point, 100.0f);

			if (!characters.empty())
			{
				if(const auto nearest = characters.front().lock())
				{
					if(nearest.get() != get_origin())
					{
						Debug::Log(L"locked on " + nearest->get_name());
						const auto diff = 
							x_velocity.get_x() < 0 ? 
								get_bottom_left() - nearest->get_bottom_left() :
								get_bottom_right() - nearest->get_bottom_right();
						m_velocity = -diff.normalized();
						m_locked_target = nearest;
						m_bLocked = true;

						if(!m_bSoundPlayed)
						{
							play_homming_sound();
							m_bSoundPlayed = true;
						}
					}
				}
			}
		}

		projectile::update();
	}

	inline void GuidedMissileProjectile::initialize()
	{
		projectile::initialize();
	}

	inline void GuidedMissileProjectile::play_hit_sound()
	{
		m_sound_pack.get_sound(L"sub-explosion").lock()->play(false);
	}

	inline void GuidedMissileProjectile::play_fire_sound()
	{
		m_sound_pack.get_sound(L"sub-fire").lock()->play(false);
	}

	inline void GuidedMissileProjectile::play_homming_sound()
	{
		m_sound_pack.get_sound(L"sub-homming").lock()->play(false);
	}

	inline void GuidedMissileProjectile::destroyed()
	{
		m_locked_target.reset();
		projectile::destroyed();
	}
}
#endif // GUIDEDMISSILEPROJECTILE_HPP
