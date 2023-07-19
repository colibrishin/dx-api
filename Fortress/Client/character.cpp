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
		m_current_projectile = m_main_projectile;
		rigidBody::initialize();
	}

	void character::hit(const std::weak_ptr<projectile>& p)
	{
		m_hp -= p.lock()->get_damage();
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

	void character::render_hp_bar(const Math::Vector2& position)
	{
		// white box
		Rectangle(
			WinAPIHandles::get_buffer_dc(),
			position.get_x(),
			position.get_y() - 20,
			position.get_x() + 52,
			position.get_y() - 10);

		// inside hp bar
		const float hp_percentage = get_hp_percentage();
		HBRUSH brush;

		if (hp_percentage > 0.5f) 
		{
			brush = CreateSolidBrush(RGB(0, 255, 0));
		}
		else if (hp_percentage >= 0.3f)
		{
			brush = CreateSolidBrush(RGB(255, 255, 0));
		}
		else
		{
			brush = CreateSolidBrush(RGB(255, 0, 0));
		}

		const RECT rect = {
			static_cast<int>(position.get_x()),
			static_cast<int>(position.get_y() - 19),
			static_cast<int>(position.get_x() + (51 * get_hp_percentage())),
			static_cast<int>(position.get_y() - 12) };

		FillRect(WinAPIHandles::get_buffer_dc(), &rect, brush);
		DeleteObject(brush);
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

			prerender();
			render_hp_bar(pos);
			
			m_current_sprite.lock()->render(pos, m_hitbox, {1, 1}, Math::to_degree(get_pitch()));

			Debug::Log(m_name + L" pos " + std::to_wstring(pos.get_x()) + L", " + std::to_wstring(pos.get_y()));
			Debug::draw_rect(pos, m_hitbox);
			Debug::draw_dot(pos);

			Debug::Log(m_name + L" pitch : " +  std::to_wstring(Math::to_degree(get_pitch())));

			Debug::Log(m_name + L" projectile : " + m_current_projectile.lock()->get_name());

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

	void character::change_projectile()
	{
		if(m_current_projectile.lock() == m_main_projectile)
		{
			m_current_projectile = m_secondary_projectile;
		}
		else
		{
			m_current_projectile = m_main_projectile;
		}
	}

	std::weak_ptr<projectile> character::get_current_projectile()
	{
		return m_current_projectile;
	}

	void character::on_collision(const CollisionCode& collision, const std::shared_ptr<rigidBody>& other)
	{
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
					Debug::Log(L"Character hits the ground");
					reset_current_gravity_speed();
					disable_gravity();
					m_bGrounded = true;
				}
				else if(ground_check == 1 || ground_check == -1)
				{
					enable_gravity();
					m_bGrounded = false;
					Debug::Log(L"Character hits the destroyed ground");
				}

				set_pitch(ground->get_top_left().local_inner_angle(get_bottom()));
			}
		}

		rigidBody::on_collision(collision, other);
	}

	void ObjectBase::character::on_nocollison()
	{
		enable_gravity();
		m_bGrounded = false;
	}

	void character::set_current_sprite(const std::wstring& name, const std::wstring& orientation)
	{
		m_current_sprite = m_texture.get_image(name, orientation);
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
	void character::prerender()
	{
	}
}
