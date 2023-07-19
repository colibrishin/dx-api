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
		void render() override;
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

	inline void GuidedMissileProjectile::render()
	{
		if(is_active())
		{
			Math::Vector2 pos{};
			const auto camera_ptr = Scene::SceneManager::get_active_scene().lock()->get_camera().lock();

			if(camera_ptr->get_locked_object().lock() == shared_from_this())
			{
				pos = camera_ptr->get_offset();
			}
			else
			{
				pos = camera_ptr->get_relative_position(std::dynamic_pointer_cast<object>(shared_from_this()));	
			}

			float angle;

			if(const auto target = m_locked_target.lock())
			{
				angle = m_position.local_inner_angle(target->get_position());
			}
			else
			{
				angle = m_position.local_inner_angle(get_fired_position());
			}

			get_current_sprite().lock()->render(
				pos,
				m_hitbox, 
				{1, 1},
				Math::to_degree(angle));

			Debug::draw_rect(pos, m_hitbox);
			Debug::draw_dot(pos);
		}
	}
	inline void GuidedMissileProjectile::unfocus_this()
	{
		m_locked_target.reset();
		projectile::unfocus_this();
	}
}
#endif // GUIDEDMISSILEPROJECTILE_HPP
