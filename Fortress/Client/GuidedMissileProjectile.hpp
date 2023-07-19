#pragma once
#ifndef GUIDEDMISSILEPROJECTILE_HPP
#define GUIDEDMISSILEPROJECTILE_HPP

#include "GifWrapper.hpp"
#include "projectile.hpp"
#include "math.h"

namespace Fortress::Object
{
	class GuidedMissileProjectile final : public ObjectBase::projectile
	{
	public:
		GuidedMissileProjectile() : projectile(
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

		if(const auto scene_ptr = Scene::SceneManager::get_active_scene().lock())
		{
			const auto characters = scene_ptr->is_in_range<ObjectBase::character>(
				x_velocity.get_x() < 0 ? get_bottom_left() : get_bottom_right(), 50.0f);

			if (!characters.empty())
			{
				if(const auto nearest = characters[0].lock())
				{
					const auto diff = get_top_left() - nearest->get_top_left();
					m_velocity = -diff.normalized();
					m_locked_target = nearest;
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
			set_pitch(m_position.local_inner_angle(target->get_position()));
		}
		else
		{
			set_pitch(m_position.local_inner_angle(get_fired_position()));
		}
	}
	
	inline void GuidedMissileProjectile::unfocus_this()
	{
		m_locked_target.reset();
		projectile::unfocus_this();
	}
}
#endif // GUIDEDMISSILEPROJECTILE_HPP
