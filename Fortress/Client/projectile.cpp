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
				if(m_curr_hit_count == m_max_hit_count)
				{
					unfocus_this();
				}

				const auto x_velocity = m_velocity * Math::Vector2{1, 0};
				const Math::Vector2 local_position = 
					x_velocity == Math::left ? get_bottom_left() : get_bottom_right() - ground->get_top_left();
				const Object::GroundState ground_check = ground->is_destroyed(
					std::floorf(local_position.get_x()), 
					std::floorf(local_position.get_y()));

				if(ground_check == Object::GroundState::NotDestroyed)
				{
					Debug::Log(L"Projectile hits the ground");
				}
				else if(ground_check == Object::GroundState::Destroyed)
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
			unfocus_this();
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

		m_curr_hit_count = 0;
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
				Math::to_degree(get_pitch()));

			Debug::draw_rect(pos, m_hitbox);
			Debug::draw_dot(pos);
		}

		rigidBody::render();
	}

	void projectile::prerender()
	{
		const auto angle = m_position.local_inner_angle(m_fired_position);

		set_pitch(angle);
	}

	void projectile::up_hit_count()
	{
		m_curr_hit_count++;
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

		set_speed(charged);

		if(x_velocity.get_x() < 0)
		{
			m_current_sprite = m_texture.get_image(L"projectile", L"left");
			const Math::Vector2 next_pos = position - Math::Vector2{m_hitbox.get_x(), 0};
			m_position = Math::Vector2{next_pos.get_x() - 5.0f, next_pos.get_y() - 5.0f};
		}
		else if (x_velocity.get_x() > 0)
		{
			m_current_sprite = m_texture.get_image(L"projectile", L"right");
			const Math::Vector2 next_pos = position + Math::Vector2{m_hitbox.get_x(), 0};
			m_position = Math::Vector2{ next_pos.get_x() + 5.0f, next_pos.get_y() - 5.0f};
		}

		m_current_sprite.lock()->play();
		m_fired_position = m_position ;
		m_velocity = velocity;
		focus_this();
	}
}
