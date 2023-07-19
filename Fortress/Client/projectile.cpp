#include "projectile.hpp"
#include "character.hpp"
#include "ground.hpp"

namespace Fortress::ObjectBase
{
	void projectile::on_collision(const CollisionCode& collison, const std::shared_ptr<Abstract::rigidBody>& other)
	{
		rigidBody::on_collision(collison, other);

		if(const auto ground = std::dynamic_pointer_cast<Object::Ground>(other))
		{
			if(ground)
			{
				const Math::Vector2 local_position = get_bottom() - ground->get_top_left();
				const int ground_check = ground->is_destroyed(
					std::floorf(local_position.get_x()), 
					std::floorf(local_position.get_y()));

				if(ground_check == 0)
				{
					Debug::Log(L"Projectile hits the ground");
					unfocus_this();
				}
				else if(ground_check == 1)
				{
					Debug::Log(L"Projectile hits the destroyed ground");
				}
			}
		}

		if (const auto character = 
				std::dynamic_pointer_cast<ObjectBase::character>(other))
		{
			Debug::Log(L"Projectile hits the character");
			character->hit(std::dynamic_pointer_cast<projectile>(shared_from_this()));
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

		m_current_sprite.lock()->reset_transfrom();
		reset_current_gravity_speed();
		reset_current_speed();
		set_disabled();
	}

	void projectile::render()
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

			const auto angle = m_position.local_inner_angle(m_fired_position);

			m_current_sprite.lock()->render(
				pos,
				m_hitbox, 
				{1, 1},
				Math::to_degree(angle));

			Debug::draw_rect(pos, m_hitbox);
			Debug::draw_dot(pos);
		}

		rigidBody::render();
	}

	int projectile::get_radius() const
	{
		return m_radius;
	}

	float projectile::get_damage() const
	{
		return m_damage;
	}

	void projectile::initialize()
	{
		m_current_sprite = m_texture.get_image(
			L"projectile",
			m_velocity * Math::Vector2{1, 0} == Math::left ? L"left" : L"right");

		m_hitbox = m_current_sprite.lock()->get_hitbox();
		rigidBody::initialize();
	}

	void projectile::fire(
		const Math::Vector2& position,
		const Math::Vector2& velocity,
		const float charged)
	{
		const auto x_velocity = velocity * Math::Vector2{1, 0};

		set_speed(charged);

		if(x_velocity.get_x() < 0)
		{
			m_current_sprite = m_texture.get_image(L"projectile", L"left");
			const Math::Vector2 next_pos = position - m_hitbox;
			m_position = Math::Vector2{next_pos.get_x() - 5.0f, next_pos.get_y() - 5.0f};
		}
		else if (x_velocity.get_x() > 0)
		{
			m_current_sprite = m_texture.get_image(L"projectile", L"right");
			m_position = Math::Vector2{position.get_x() + 5.0f, position.get_y() - 5.0f};
		}

		m_current_sprite.lock()->play();
		m_hitbox = m_current_sprite.lock()->get_hitbox();
		m_fired_position = m_position ;
		m_velocity = velocity;
		focus_this();
	}
}
