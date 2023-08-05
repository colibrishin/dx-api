#include "character.hpp"

#include "deltatime.hpp"
#include "DoubleShotItem.hpp"
#include "ground.hpp"
#include "input.hpp"
#include "projectile.hpp"
#include "TeleportItem.hpp"
#include "NutshellProjectile.hpp"
#include "RepairItem.hpp"

namespace Fortress::ObjectBase
{
	void character::initialize()
	{
		set_current_sprite(eCharacterAnim::Idle);
		set_state(eCharacterState::Idle);
		m_current_projectile = m_main_projectile;
		rigidBody::initialize();
		m_nutshell_projectile = std::make_shared<Object::NutShellProjectile>(this);
		m_available_items.emplace(1, std::make_shared<Item::DoubleShotItem>());
		m_available_items.emplace(2, std::make_shared<Item::TeleportItem>());
		m_available_items.emplace(3, std::make_shared<Item::RepairItem>());
		m_main_projectile->set_disabled();
		m_secondary_projectile->set_disabled();
	}

	void character::hit(const std::weak_ptr<projectile>& p)
	{
		m_hp -= p.lock()->get_damage();
		post_hit();
	}

	void character::hit(const float damage)
	{
		m_hp -= damage;
		post_hit();
	}

	void character::post_hit()
	{
		set_state(eCharacterState::Hit);
	}

	void character::shoot()
	{
		m_current_projectile.lock()->play_fire_sound();
		m_current_projectile.lock()->set_enabled();
	}

	float character::get_charged_power() const
	{
		return m_power;
	}

	float character::get_hp_percentage() const
	{
		if(m_hp <= 0.0f)
		{
			return 0.0f;
		}

		return m_hp / static_cast<float>(character_full_hp);
	}

	float character::get_mp_percentage() const
	{
		if(m_hp <= 0.0f)
		{
			return 0.0f;
		}

		return m_mp / static_cast<float>(character_full_mp);
	}

	float character::get_hp_raw() const
	{
		return m_hp;
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

	void character::render()
	{
		Math::Vector2 pos{};

		if (is_active())
		{
			const auto camera_ptr = Scene::SceneManager::get_active_scene().lock()->get_camera().lock();

			if(camera_ptr->get_locked_object().lock() == std::dynamic_pointer_cast<object>(shared_from_this()))
			{
				pos = camera_ptr->get_offset(m_hitbox);
			}
			else
			{
				pos = camera_ptr->get_relative_position(
					std::dynamic_pointer_cast<object>(shared_from_this()));
			}

			prerender();
			render_hp_bar(pos);
			
			m_current_sprite.lock()->render(pos, m_hitbox, {1, 1}, Math::to_degree(get_movement_pitch_radian()));
			
			Debug::draw_rect(pos, m_hitbox);
			Debug::draw_dot(camera_ptr->get_offset(m_hitbox));

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
		if(get_charged_power() < character_max_charge)
		{
			m_power += 100.0f * DeltaTime::get_deltaTime();
		}
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

	void character::equip_nutshell()
	{
		m_tmp_projectile = m_current_projectile;
		m_current_projectile = std::dynamic_pointer_cast<projectile>(m_nutshell_projectile);
	}

	std::weak_ptr<projectile> character::get_current_projectile()
	{
		return m_current_projectile;
	}

	const std::wstring& character::get_short_name() const
	{
		return m_shot_name;
	}

	void character::set_hp(const float hp)
	{
		if(hp >= character_full_hp)
		{
			m_hp = character_full_hp;
		}
		else
		{
			m_hp = hp;
		}
	}

	void character::set_unmovable()
	{
		m_bMovable = false;
	}

	void character::set_movable()
	{
		m_bMovable = true;
	}

	void character::unequip_nutshell()
	{
		if(m_current_projectile.lock() == m_nutshell_projectile && m_tmp_projectile.lock())
		{
			m_current_projectile = m_tmp_projectile;
		}
	}

	void character::reset_mp()
	{
		m_mp = character_full_mp;
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

	void character::move_left()
	{
		rigidBody::move_left();
	}

	void character::move_right()
	{
		rigidBody::move_right();
	}

	void character::stop()
	{
		if(const auto move_sound = m_sound_pack.get_sound(L"move").lock())
		{
			if(move_sound->is_playing())
			{
				move_sound->stop(true);
			}
		}
		rigidBody::stop();
	}
}
