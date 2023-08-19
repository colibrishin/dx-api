#include "pch.h"
#include "projectile.hpp"

#include "BattleScene.h"
#include "character.hpp"
#include "deltatime.hpp"
#include "ground.hpp"
#include "Round.h"

namespace Fortress::ObjectBase
{
	void projectile::update()
	{
		rigidBody::update();
		ProjectileController::update();
	}

	void projectile::on_collision(const CollisionCode& collision, const GlobalPosition& collision_point, const std::weak_ptr<Abstract::rigidBody>& other)
	{
		if(const auto prj = other.lock()->downcast_from_this<projectile>())
		{
			return;
		}

		if(const auto ground = other.lock()->downcast_from_this<Object::Ground>())
		{
			if (get_max_hit_count() > get_hit_count() &&
				ground->safe_is_projectile_hit(collision_point, rigidBody::downcast_from_this<projectile>()))
			{
				notify_ground_hit();
				ground->safe_set_destroyed_global(collision_point, get_radius());
			}
		}

		if(const auto ch  = other.lock()->downcast_from_this<character>())
		{
			notify_character_hit();
		}
	}

	void projectile::render()
	{
		if(is_active())
		{
			Math::Vector2 pos{};
			const auto camera_ptr = Scene::SceneManager::get_active_scene().lock()->get_camera().lock();

			if(camera_ptr->get_locked_object().lock() == rigidBody::shared_from_this())
			{
				pos = camera_ptr->get_offset(m_hitbox);
			}
			else
			{
				pos = camera_ptr->get_relative_position(
					std::dynamic_pointer_cast<object>(rigidBody::shared_from_this()));	
			}

			prerender();

			m_current_sprite.lock()->render(
				pos,
				m_hitbox, 
				{1, 1},
				Math::to_degree(get_movement_pitch_radian()));
		}

		rigidBody::render();
	}

	void projectile::prerender()
	{
		ProjectileController::prerender();
		set_movement_pitch_radian(get_pitch());
	}

	const character* projectile::get_origin() const
	{
		return m_shooter;
	}

	float projectile::get_radius() const
	{
		return m_radius;
	}

	float projectile::get_damage() const
	{
		return m_damage;
	}

	float projectile::get_penetration_rate() const
	{
		return m_armor_penetration;
	}

	projectile::projectile(
			const character* shooter,
			const std::wstring& name,
			const std::wstring& short_name,
			const Math::Vector2& position,
			const Math::Vector2& velocity,
			const float mass,
			const Math::Vector2& speed,
			const Math::Vector2& acceleration,
			const float damage,
			const float radius,
			const int hit_count,
			const int fire_count,
			const float armor_penetration) :
			rigidBody(name, position, {30.0f, 30.0f}, velocity, mass, speed, acceleration, true),
			ProjectileController(short_name, this, hit_count, fire_count),
			m_damage(damage),
			m_radius(radius),
			m_armor_penetration(armor_penetration),
			m_wind_acceleration(),
			m_shooter(shooter)
	{
	}

	void projectile::move()
	{
		rigidBody::move();
	}

	void projectile::fire()
	{
		reset_current_gravity_speed();
		reset_current_speed();
		set_enabled();
	}

	void projectile::flying()
	{
		modify_current_speed(m_wind_acceleration);
	}

	void projectile::hit()
	{
		if (const auto scene = Scene::SceneManager::get_active_scene().lock())
		{
			const auto characters = scene->is_in_range_nearest<character>(
				rigidBody::downcast_from_this<object>(), get_radius());
			const auto grounds = scene->get_objects<Object::Ground>();

			for(const auto& ptr : grounds)
			{
				if (const auto ground = ptr.lock())
				{
					ground->safe_set_destroyed_global(get_center(), get_radius());
				}
			}

			for (const auto& [ptr, _] : characters)
			{
				if (const auto character = ptr.lock())
				{
					character->hit(
						rigidBody::downcast_from_this<projectile>(), get_center());
				}
			}
		}
	}

	void projectile::destroyed()
	{
		reset_current_gravity_speed();
		reset_current_speed();
		set_disabled();
	}

	Math::Vector2 projectile::projectile_speed_getter(const std::wstring& short_name, const std::wstring& type)
	{
		const Math::Vector2 default_projectile_speed = Math::Vector2{2000.0f, 1.0f};

		if(short_name == L"cannon" && type == L"main")
		{
			return default_projectile_speed * 3;
		}
		if(short_name == L"cannon" && type == L"sub")
		{
			return default_projectile_speed * 5;
		}
		if(short_name == L"missile" && type == L"main")
		{
			return default_projectile_speed * 2;
		}
		if(short_name == L"missile" && type == L"sub")
		{
			return default_projectile_speed * 2;
		}
		if(short_name == L"secwind" && type == L"main")
		{
			return default_projectile_speed * 1.5;
		}
		if(short_name == L"secwind" && type == L"sub")
		{
			return default_projectile_speed * 5;
		}

		return {};
	}

	void projectile::initialize()
	{
		rigidBody::initialize();
		ProjectileController::initialize();
	}

	void projectile::fire(
		const Math::Vector2& position,
		const Math::Vector2& velocity,
		const float charged)
	{
		set_speed({charged, charged});
		m_position = position;

		if(const auto battle_scene = 
			std::dynamic_pointer_cast<Scene::BattleScene>(Scene::SceneManager::get_active_scene().lock()))
		{
			m_wind_acceleration = {
				battle_scene->get_round_status().lock()->get_wind_acceleration(),
				0.0f};
		}

		m_velocity = velocity;
	}
}
