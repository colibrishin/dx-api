#include "projectile.hpp"
#include "character.hpp"
#include "ground.hpp"

namespace Fortress::ObjectBase
{
	void projectile::on_collision(const std::shared_ptr<Abstract::rigidBody>& other)
	{
		bool hit = false;

		if (auto gr = std::dynamic_pointer_cast<Object::Ground>(other))
		{
			// @todo: dig a hole into the Ground.
			Debug::Log(L"Projectile hits the Ground");
			hit = true;
		}

		if (const auto ch = std::dynamic_pointer_cast<character>(other))
		{
			Debug::Log(L"Projectile hits the character");
			hit = true;
			ch->hit(this);
		}

		if (hit)
		{
			m_gravity_speed = 0.0f;
			set_disabled();
		}

		rigidBody::on_collision(other);
		unfocus_this();
	}

	void projectile::focus_this()
	{
		Scene::SceneManager::get_active_scene()->add_game_object(
			Abstract::LayerType::Character, std::dynamic_pointer_cast<object>(shared_from_this()));
		Scene::SceneManager::get_active_scene()->get_camera()->set_object(std::dynamic_pointer_cast<object>(shared_from_this()));

		reset_current_gravity_speed();
		reset_current_speed();
		set_enabled();
	}

	void projectile::unfocus_this()
	{
		Scene::SceneManager::get_active_scene()->remove_game_object(
			Abstract::LayerType::Character, std::dynamic_pointer_cast<object>(shared_from_this()));
		Scene::SceneManager::get_active_scene()->get_camera()->restore_object();

		reset_current_gravity_speed();
		reset_current_speed();
		set_disabled();
	}

	void projectile::render()
	{
		if(is_active())
		{
			Math::Vector2 pos{};

			if(Scene::SceneManager::get_active_scene()->get_camera()->get_locked_object() == shared_from_this())
			{
				pos = Scene::SceneManager::get_active_scene()->get_camera()->get_offset();
			}
			else
			{
				pos = Scene::SceneManager::get_active_scene()->get_camera()->get_relative_position(std::dynamic_pointer_cast<object>(shared_from_this()));	
			}

			m_current_sprite->render(pos, m_hitbox);
			Debug::draw_rect(pos, m_hitbox);
			Debug::draw_dot(pos);
		}

		rigidBody::render();
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

		m_hitbox = m_current_sprite->get_hitbox();
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
			m_position = Math::Vector2{position.get_x() - m_hitbox.get_x() - 5.0f, position.get_y() - 5.0f};
		}
		else if (x_velocity.get_x() > 0)
		{
			m_current_sprite = m_texture.get_image(L"projectile", L"right");
			m_position = Math::Vector2{position.get_x() + 5.0f, position.get_y() - 5.0f};
		}

		m_current_sprite->play();
		m_hitbox = m_current_sprite->get_hitbox();
		m_velocity = velocity;
		focus_this();
	}
}
