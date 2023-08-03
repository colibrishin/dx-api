#include "projectile.hpp"

#include "BattleScene.h"
#include "character.hpp"
#include "deltatime.hpp"
#include "ground.hpp"
#include "Round.hpp"

namespace Fortress::ObjectBase
{
	void projectile::post_hit()
	{
		m_curr_hit_count = 0;
		m_current_sprite.lock()->reset_transfrom();
		reset_current_gravity_speed();
		reset_current_speed();
		set_disabled();
	}

	void projectile::on_collision(const CollisionCode& collision, const Math::Vector2& hit_vector, const std::weak_ptr<Abstract::rigidBody>& other)
	{
		rigidBody::on_collision(collision, hit_vector, other);

		if(m_curr_hit_count == m_max_hit_count)
		{
			post_hit();
			return;
		}

		if (const auto character = 
				std::dynamic_pointer_cast<ObjectBase::character>(other.lock()))
		{
			const auto shared_this = std::dynamic_pointer_cast<projectile>(shared_from_this());
			Debug::Log(L"Projectile hits the character");
			character->hit(shared_this);
			up_hit_count();
			play_hit_sound();
			explosion_near_ground(hit_vector);
		}
	}

	void projectile::focus_this()
	{
		const auto scene_ptr = Scene::SceneManager::get_active_scene().lock();

		scene_ptr->add_game_object(
			Abstract::LayerType::Character, std::dynamic_pointer_cast<object>(shared_from_this()));
		scene_ptr->get_camera().lock()->set_object(std::dynamic_pointer_cast<object>(shared_from_this()));

		reset_current_gravity_speed();
		reset_current_speed();
		set_enabled();
	}

	void projectile::unfocus_this()
	{
		const auto scene_ptr = Scene::SceneManager::get_active_scene().lock();

		scene_ptr->remove_game_object(
			Abstract::LayerType::Character, std::dynamic_pointer_cast<object>(shared_from_this()));
		scene_ptr->get_camera().lock()->restore_object();
		post_hit();
	}

	void projectile::render()
	{
		if(is_active())
		{
			Math::Vector2 pos{};
			const auto camera_ptr = Scene::SceneManager::get_active_scene().lock()->get_camera().lock();

			if(camera_ptr->get_locked_object().lock() == shared_from_this())
			{
				pos = camera_ptr->get_offset(m_hitbox);
			}
			else
			{
				pos = camera_ptr->get_relative_position(std::dynamic_pointer_cast<object>(shared_from_this()));	
			}

			prerender();

			m_current_sprite.lock()->render(
				pos,
				m_hitbox, 
				{1, 1},
				Math::to_degree(get_movement_pitch_radian()));

			Debug::draw_rect(pos - m_hitbox / 2, m_hitbox);
			Debug::draw_dot(pos);
		}

		rigidBody::render();
	}

	void projectile::prerender()
	{
		static Math::Vector2 previous_position = m_fired_position;

		const auto unit = (m_position - previous_position).normalized();
		const float radian = atan2(unit.get_y(), unit.get_x());
		set_movement_pitch_radian(get_velocity_offset() == Math::left ? Math::flip_radian(radian) : radian);
		previous_position = m_position;
	}

	const character* projectile::get_origin() const
	{
		return m_shooter;
	}

	void projectile::up_hit_count()
	{
		m_curr_hit_count++;
	}

	int projectile::get_hit_count() const
	{
		return m_curr_hit_count;
	}

	int projectile::get_max_hit_count() const
	{
		return m_max_hit_count;
	}

	const std::weak_ptr<GifWrapper>& projectile::get_current_sprite() const
	{
		return m_current_sprite;
	}

	int projectile::get_radius() const
	{
		return m_radius;
	}

	float projectile::get_damage() const
	{
		return m_damage;
	}

	const Math::Vector2& projectile::get_fired_position() const
	{
		return m_fired_position;
	}

	void projectile::explosion_near_ground(const Math::Vector2& victim_hit_vector) const
	{
		if (const auto active_scene =
			Scene::SceneManager::get_active_scene().lock())
		{
			// reverse the hit vector because hit vector is based on victim.
			const eHitVector e_vec = translate_hit_vector(-victim_hit_vector);
			const auto hit_point = get_hit_point(e_vec);

			const auto grounds = active_scene->is_in_range<Object::Ground>(
				hit_point,
				get_radius());

			for (const auto& ptr : grounds)
			{
				if (const auto ground = ptr.lock())
				{
					// using origin collision point of projectile as explosion mid point.
					ground->safe_set_destroyed_global(hit_point, get_radius());
				}
			}
		}
	}

	Math::Vector2 projectile::projectile_speed_getter(const std::wstring& short_name, const std::wstring& type)
	{
		if(short_name == L"cannon" && type == L"main")
		{
			return {100.0f, 1.0f};
		}
		if(short_name == L"cannon" && type == L"sub")
		{
			return {300.0f, 1.0f};
		}
		if(short_name == L"missile" && type == L"main")
		{
			return {200.0f, 1.0f};
		}
		if(short_name == L"missile" && type == L"sub")
		{
			return {200.0f, 1.0f};
		}

		return {};
	}

	void projectile::initialize()
	{
		m_current_sprite = m_texture.get_image(
			L"projectile",
			m_velocity * Math::Vector2{1, 0} == Math::left ? L"left" : L"right");
		rigidBody::initialize();
	}

	void projectile::fire(
		const Math::Vector2& position,
		const Math::Vector2& velocity,
		const float charged)
	{
		const auto x_velocity = velocity * Math::Vector2{1, 0};

		set_speed({charged, charged});

		if(x_velocity.get_x() < 0)
		{
			m_current_sprite = m_texture.get_image(L"projectile", L"left");
			m_position = position - Math::Vector2{m_hitbox.get_x(), 0};
		}
		else if (x_velocity.get_x() > 0)
		{
			m_current_sprite = m_texture.get_image(L"projectile", L"right");
			m_position = position + Math::Vector2{m_hitbox.get_x(), 0};
		}

		if(const auto battle_scene = 
			std::dynamic_pointer_cast<Scene::BattleScene>(Scene::SceneManager::get_active_scene().lock()))
		{
			m_acceleration = {battle_scene->get_round_status().get_wind_acceleration(), 1.0f};
		}

		m_current_sprite.lock()->play();
		m_fired_position = m_position ;
		m_velocity = velocity;
		focus_this();
	}
}
