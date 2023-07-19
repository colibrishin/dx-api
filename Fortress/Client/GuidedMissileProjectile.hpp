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

	private:
		Math::Vector2 m_fired_position;
	};

	inline void GuidedMissileProjectile::update()
	{
		if(const auto scene_ptr = Scene::SceneManager::get_active_scene().lock())
		{
			const auto characters = scene_ptr->is_in_range<ObjectBase::character>(
				m_position,
				m_hitbox,
				100.0f);

			// @todo: this is not positionally nearest but order-wise nearest. 
			if (!characters.empty())
			{
				if(const auto nearest = characters[0].lock())
				{
					const auto diff = m_position - nearest->get_position();
					m_velocity = -diff.normalized();
				}
			}
		}

		projectile::update();
	}

	inline void GuidedMissileProjectile::initialize()
	{
		projectile::initialize();
	}
}
#endif // GUIDEDMISSILEPROJECTILE_HPP
