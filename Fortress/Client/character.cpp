#include "character.hpp"

#include "deltatime.hpp"
#include "DoubleShotItem.hpp"
#include "ground.hpp"
#include "input.hpp"
#include "projectile.hpp"
#include "TeleportItem.hpp"
#include "NutshellProjectile.hpp"
#include "RepairItem.hpp"

#undef max

namespace Fortress::ObjectBase
{
	void character::initialize()
	{
		set_current_sprite(eCharacterAnim::Idle);
		set_state(eCharacterState::Idle);
		rigidBody::initialize();
		m_available_items.emplace(1, std::make_shared<Item::DoubleShotItem>());
		m_available_items.emplace(2, std::make_shared<Item::TeleportItem>());
		m_available_items.emplace(3, std::make_shared<Item::RepairItem>());
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
		m_multi_projectile_timer.reset();

		float charged = get_charged_power();

		if(charged <= 0.0f)
		{
			charged = 10.0f;
		}

		Math::Vector2 angle{};

		if(get_offset() == Math::left)
		{
			angle = {-cosf(get_movement_pitch_radian()), -sinf(get_movement_pitch_radian())};
		}
		else
		{
			angle = {cosf(get_movement_pitch_radian()), sinf(get_movement_pitch_radian())};
		}

		const std::weak_ptr<projectile> instantiated = initialize_projectile(angle, charged);
		instantiated.lock()->play_fire_sound();

		const int remaining = instantiated.lock()->get_fire_count() - 1;

		if(remaining > 0)
		{
			m_multi_projectile_timer.set_count(remaining);
			m_multi_projectile_timer.start([this, angle, charged](){initialize_projectile(angle, charged);});
		}
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
		if(m_projectile_type == eProjectileType::Main)
		{
			m_projectile_type = eProjectileType::Sub;
		}
		else if(m_projectile_type == eProjectileType::Sub)
		{
			m_projectile_type = eProjectileType::Main;
		}
	}

	void character::equip_nutshell()
	{
		m_tmp_projectile_type = m_projectile_type;
		m_projectile_type = eProjectileType::Nutshell;
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
		if(m_tmp_projectile_type != eProjectileType::Nutshell && m_projectile_type == eProjectileType::Nutshell)
		{
			m_projectile_type = m_tmp_projectile_type;
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

	std::weak_ptr<projectile> character::initialize_projectile(const Math::Vector2& angle, const float charged)
	{
		std::weak_ptr<projectile> instantiated;

		if(m_projectile_type == eProjectileType::Main)
		{
			instantiated = get_main_projectile();
		}
		else if(m_projectile_type == eProjectileType::Sub)
		{
			instantiated = get_sub_projectile();
		}
		else
		{
			instantiated = ObjectManager::create_object<Object::NutShellProjectile>(this);
		}

		const auto projectile = instantiated.lock();

		const auto forward = Math::Vector2{get_offset().get_x(), -1} * projectile->m_hitbox.get_x();
		const auto forward_rotation = forward.rotate(get_movement_pitch_radian());

		projectile->fire(
			(get_offset() == Math::left ? get_top_left() : get_top_right()) + forward_rotation, 
			angle, 
			charged);

		if(const auto scene = Scene::SceneManager::get_active_scene().lock())
		{
			scene->add_game_object(Abstract::LayerType::Projectile, projectile);
		}

		return instantiated;
	}

	eProjectileType character::get_projectile_type() const
	{
		return m_projectile_type;
	}

	bool character::is_projectile_fire_counted() const
	{
		const auto scene = Scene::SceneManager::get_active_scene().lock();
		const auto projectile_list = scene->get_objects<projectile>();

		if(projectile_list.empty())
		{
			return false;
		}

		int exploded = 0;
		int fire_count = 0;

		for(const auto& prj : projectile_list)
		{
			if(const auto projectile = prj.lock())
			{
				fire_count = std::max(fire_count, projectile->get_fire_count());

				if(projectile->is_exploded() && projectile->get_origin() == this)
				{
					exploded++;
				}
			}
		}

		return fire_count == exploded;
	}

	bool character::is_projectile_active() const
	{
		const auto scene = Scene::SceneManager::get_active_scene().lock();
		const auto projectile_list = scene->get_objects<projectile>();

		if(projectile_list.empty())
		{
			return false;
		}

		int active_count = 0;

		for(const auto& prj : projectile_list)
		{
			if(const auto projectile = prj.lock())
			{
				if(projectile->is_active() && projectile->get_origin() == this)
				{
					active_count++;
				}
			}
		}

		return active_count != 0;
	}

	std::weak_ptr<projectile> character::get_one_active_projectile() const
	{
		const auto scene = Scene::SceneManager::get_active_scene().lock();
		const auto projectile_list = scene->get_objects<projectile>();

		if(projectile_list.empty())
		{
			return {};
		}

		for(const auto& prj : projectile_list)
		{
			if(const auto projectile = prj.lock())
			{
				if(projectile->is_active() && projectile->get_origin() == this)
				{
					return prj;
				}
			}
		}

		return {};
	}

	std::vector<std::weak_ptr<projectile>> character::get_projectiles() const
	{
		const auto scene = Scene::SceneManager::get_active_scene().lock();
		const auto projectile_list = scene->get_objects<projectile>();
		std::vector<std::weak_ptr<projectile>> ret = {};

		for(const auto& cand : projectile_list)
		{
			if(const auto prj = cand.lock())
			{
				if(prj->get_origin() == this)
				{
					ret.push_back(cand);
				}
			}
		}

		return ret;
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
