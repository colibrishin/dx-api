#pragma once
#ifndef GUIDEDMISSILEPROJECTILE_HPP
#define GUIDEDMISSILEPROJECTILE_HPP

#include "GifWrapper.hpp"
#include "projectile.hpp"
#include "math.h"

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
			2.0f, 
			0.0f, 
			10.0f,
			10,
			1)
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
		virtual void prerender() override;
		void unfocus_this() override;

	private:
		std::weak_ptr<ObjectBase::character> m_locked_target;
	};

	inline void GuidedMissileProjectile::update()
	{
		const auto x_velocity = m_velocity * Math::Vector2{1, 0};
		const auto detection_point = x_velocity.get_x() < 0 ? get_bottom_left() : get_bottom_right();

		if(const auto scene_ptr = Scene::SceneManager::get_active_scene().lock())
		{
			// @todo: if radius is big enough to contains shooter itself then missile is moving back and forth.
			const auto characters = scene_ptr->is_in_range<ObjectBase::character>(
				detection_point, 10.0f);

			if (!characters.empty())
			{
				if(const auto nearest = characters.front().lock())
				{
					if(nearest.get() != get_origin())
					{
						const auto diff = 
							x_velocity.get_x() < 0 ? 
								get_bottom_left() - nearest->get_top_left() :
								get_bottom_right() - nearest->get_top_right();
						m_velocity = -diff.normalized();
						m_locked_target = nearest;
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

	inline void GuidedMissileProjectile::prerender()
	{
		if (const auto target = m_locked_target.lock())
		{
			set_movement_pitch_radian(m_position.local_inner_angle(target->get_position()));
		}
		else
		{
			set_movement_pitch_radian(m_position.local_inner_angle(get_fired_position()));
		}
	}
	
	inline void GuidedMissileProjectile::unfocus_this()
	{
		m_locked_target.reset();
		projectile::unfocus_this();
	}
}
#endif // GUIDEDMISSILEPROJECTILE_HPP
