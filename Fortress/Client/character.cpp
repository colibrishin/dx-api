#include "character.hpp"

#include "deltatime.hpp"
#include "ground.hpp"
#include "projectile.hpp"

namespace Fortress::ObjectBase
{
	void character::initialize()
	{
		m_hitbox = m_texture.get_image(L"idle", L"left").lock()->get_hitbox();
		set_current_sprite(L"idle", m_offset == Math::left ? L"left" : L"right");
		rigidBody::initialize();
	}

	void character::hit(const projectile* p)
	{
		m_hp -= p->get_damage();
	}

	void character::shoot()
	{
		set_current_sprite(L"fire", m_offset == Math::left ? L"left" : L"right");

		// @todo: maybe queue?
		m_current_sprite.lock()->play([this]()
		{
			set_current_sprite(L"idle", m_offset == Math::left ? L"left" : L"right");
		});
		m_power = 1.0f;
	}

	float character::get_charged_power() const
	{
		return m_power;
	}

	float character::get_hp_percentage() const
	{
		return m_hp / static_cast<float>(character_full_hp);
	}

	float character::get_mp_percentage() const
	{
		return m_mp / static_cast<float>(character_full_mp);
	}

	void character::update()
	{
		rigidBody::update();
	}

	void character::render()
	{
		Math::Vector2 pos{};

		if (is_active())
		{
			const auto camera_ptr = Scene::SceneManager::get_active_scene().lock()->get_camera().lock();

			if(camera_ptr->get_locked_object().lock() == std::dynamic_pointer_cast<object>(shared_from_this()))
			{
				pos = camera_ptr->get_offset();
			}
			else
			{
				pos = camera_ptr->get_relative_position(
					std::dynamic_pointer_cast<object>(shared_from_this()));
			}

			m_current_sprite.lock()->render(pos, m_hitbox);

			Debug::Log(L"Char pos" + std::to_wstring(pos.get_x()) + std::to_wstring(pos.get_y()));
			Debug::draw_rect(pos, m_hitbox);
			Debug::draw_dot(pos);

			Debug::Log(L"Angle : " +  std::to_wstring(camera_ptr->get_offset().local_inner_angle(pos)));

			// c
			Debug::draw_line(pos, camera_ptr->get_offset());

			// t
			Debug::draw_line(
				camera_ptr->get_offset(), 
				{camera_ptr->get_offset().get_x(), pos.get_y()});

			// s
			Debug::draw_line(
				{camera_ptr->get_offset().get_x(), pos.get_y()}, pos);
		}

		rigidBody::render();
	}

	void character::firing()
	{
		if(get_current_sprite_name().find(L"charging") == std::wstring::npos)
		{
			set_current_sprite(L"charging", m_offset == Math::left ? L"left" : L"right");
		}

		m_power += 100.0f * DeltaTime::get_deltaTime();

		Debug::Log(L"Power : " + std::to_wstring(m_power));
	}

	void character::move()
	{
		if (m_mp < Math::epsilon)
		{
			return;
		}

		if (std::fabs(m_velocity.get_x()) > Math::epsilon)
		{
			m_mp -= 20.0f * DeltaTime::get_deltaTime();
		}

		rigidBody::move();
	}

	void character::on_collision(const std::shared_ptr<Abstract::rigidBody>& other)
	{
		if(const auto ground = std::dynamic_pointer_cast<Object::Ground>(other))
		{
			if(ground)
			{
				Debug::Log(L"Character hits the ground");
				reset_current_gravity_speed();
				disable_gravity();
				m_bGrounded = true;
				return;
			}
		}

		rigidBody::on_collision(other);
	}

	void ObjectBase::character::on_nocollison()
	{
		enable_gravity();
		m_bGrounded = false;
	}

	void character::set_current_sprite(const std::wstring& name, const std::wstring& orientation)
	{
		m_current_sprite = m_texture.get_image(name, orientation);
		m_current_sprite.lock()->play();
	}

	void character::set_sprite_offset(const std::wstring& name, const std::wstring& orientation,
		const Math::Vector2& offset)
	{
		m_texture.get_image(name, orientation).lock()->set_offset(offset);
	}

	const std::wstring& character::get_current_sprite_name() const
	{
		return m_current_sprite.lock()->get_name();
	}

	Math::Vector2 character::get_offset() const
	{
		return m_offset;
	}

	void character::move_left()
	{
		m_offset = Math::left;
		set_current_sprite(L"move", L"left");
		rigidBody::move_left();
	}

	void character::move_right()
	{
		m_offset = Math::right;
		set_current_sprite(L"move", L"right");
		rigidBody::move_right();
	}

	void character::stop()
	{
		set_current_sprite(L"idle", m_offset == Math::left ? L"left" : L"right");
		rigidBody::stop();
	}
}
