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
		push_sprite(eCharacterAnimation::Idle, false);
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
		if(get_hp_percentage() > 0.0f)
		{
			push_sprite(eCharacterAnimation::Hit, true);
			push_sprite(eCharacterAnimation::Idle, false);
		}
		else
		{
			push_sprite(eCharacterAnimation::Death, true);
			push_sprite(eCharacterAnimation::Dead, false);
			set_state(eCharacterState::Dead);
		}
	}

	void character::shoot()
	{
		push_sprite(eCharacterAnimation::Fire, false);
		m_current_projectile.lock()->play_fire_sound();
		m_current_projectile.lock()->set_enabled();
		push_sprite(eCharacterAnimation::Idle, false);
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

	void character::set_state(const eCharacterState& state)
	{
		m_state = state;
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

	void character::ground_walk(const CollisionCode& collision, const std::weak_ptr<Object::Ground>& ptr_ground)
	{
		if(const auto ground = ptr_ground.lock())
		{
			const Math::Vector2 bottom_local_position = ground->to_top_left_local_position(get_bottom());

			const Object::GroundState bottom_check = ground->safe_is_destroyed(bottom_local_position);

			if(collision == CollisionCode::Inside)
			{
				if (get_state() == eCharacterState::Move) 
				{
					if (bottom_check == Object::GroundState::NotDestroyed)
					{
						if (ground->safe_is_object_stuck_global(get_bottom()))
						{
							const auto delta = ground->safe_nearest_surface(get_bottom());
							m_position -= delta;
						}
					}

					const auto candidate = get_next_velocity(bottom_local_position, ground);

					if (candidate == Math::Vector2{})
					{
						return;
					}

					if (m_bGrounded && candidate != Math::vector_inf)
					{
						m_velocity = candidate;
					}
				}
			}
		}
	}

	void character::ground_gravity(const std::weak_ptr<Object::Ground>& ptr_ground)
	{
		if(const auto ground = ptr_ground.lock())
		{
			const auto bottom_check = ground->safe_is_destroyed(ground->to_top_left_local_position(get_bottom()));

			if(bottom_check == Object::GroundState::NotDestroyed)
			{
				reset_current_gravity_speed();
				disable_gravity();
				m_bGrounded = true;
			}
			else if (bottom_check == Object::GroundState::Destroyed)
			{
				enable_gravity();
				m_bGrounded = false;
				set_movement_pitch_radian(0.0f);
				Debug::Log(L"Character hits the destroyed ground");
			}
			else if (bottom_check == Object::GroundState::OutOfBound)
			{
				enable_gravity();
				m_bGrounded = false;
				set_movement_pitch_radian(0.0f);
				Debug::Log(L"Character is outside of the ground");
			}
		}
	}

	void character::ground_pitching(const std::weak_ptr<Object::Ground>& ptr_ground)
	{
		if(const auto ground = ptr_ground.lock())
		{
			auto next_surface = get_offset_bottom_forward_position();
			Math::Vector2 delta{};

			// @todo: this still needs to be fixed. inner destruction affects pitch.
			for(int x = 0; x < m_hitbox.get_x(); ++x)
			{
				delta = ground->safe_orthogonal_surface_zero_global(
					next_surface + 
					(get_offset() == Math::left ? Math::right : Math::left) * x);

				if(delta != Math::vector_inf)
				{
					delta += (get_offset() == Math::left ? Math::right : Math::left) * x;
					break;
				}
			}

			if(delta == Math::vector_inf)
			{
				set_movement_pitch_radian(0);
				return;
			}

			// this also means that this is uphilling.
			if(ground->safe_is_object_stuck_global(get_offset_bottom_forward_position()))
			{
				next_surface -= delta;
			}
			else
			{
				next_surface += delta;
			}

			Debug::draw_dot(next_surface);
			auto rotate_radian = next_surface.local_inner_angle(get_bottom());

			set_movement_pitch_radian(rotate_radian);
		}
	}

	Math::Vector2 character::get_next_velocity(
		const Math::Vector2& local_position_bottom, const std::weak_ptr<Object::Ground>& ground_ptr) const
	{
		bool angle_check = false;
		bool climable = false;
		Math::Vector2 candidate{};

		if (const auto ground = ground_ptr.lock())
		{
			// check up-hilling condition
			for(int x = 0; x < 10; x++)
			{
				if (angle_check && climable) 
				{
					break;
				}

				angle_check = false;

				// searching in reverse. we need to check whether how stiff the curve is.
				for(int y = 99; y >= 0; --y)
				{
					Math::Vector2 local_new_pos = {
						local_position_bottom.get_x() + 
							(get_offset() == Math::left ?  -x : x),
						local_position_bottom.get_y() - y
					};

					auto diff = local_new_pos - local_position_bottom;
					if(get_offset() == Math::left)
					{
						diff += ground->m_hitbox;
					}
					const auto unit = diff.normalized();
					const auto ground_check = ground->safe_is_destroyed(local_new_pos);

					if (isnan(unit.get_x()) || isnan(unit.get_y()) || 
						ground_check == Object::GroundState::OutOfBound)
					{
						continue;
					}

					if (ground_check == Object::GroundState::NotDestroyed)
					{
						if (!angle_check)
						{
							if (std::fabs(Math::to_degree(local_position_bottom.local_inner_angle(local_new_pos)))
								>= 60.0f || ground->safe_is_object_stuck_local(local_new_pos)) 
							{
								// @todo: fixed animation
								angle_check = false;
								climable = false;
								candidate = Math::vector_inf;
							}
							
							climable = true;
							angle_check = true;
						}

						candidate = unit;
					}
				}
			}

			if(climable)
			{
				return candidate;
			}

			candidate = {};

			for(int i = 0; i < 10; i++)
			{
				for(int j = 0; j < 100; ++j)
				{
					Math::Vector2 local_new_pos = {
						local_position_bottom.get_x() + 
							(get_offset() == Math::left ?  -i : i),
						local_position_bottom.get_y() + j
					};

					auto diff = local_new_pos - local_position_bottom;
					if(get_offset() == Math::left)
					{
						diff += ground->m_hitbox;
					}
					const auto unit = diff.normalized();
					const auto ground_check = ground->safe_is_destroyed(local_new_pos);

					if (isnan(unit.get_x()) || isnan(unit.get_y()))
					{
						continue;
					}

					if (ground_check == Object::GroundState::NotDestroyed)
					{
						candidate = unit;
						return candidate;
					}
				}
			}
		}

		if (!angle_check || !climable) 
		{
			candidate = Math::vector_inf;
		}

		return candidate;
	}

	void character::update()
	{
		if(!m_animation_queue.empty())
		{
			if(const auto next = m_animation_queue.update().lock())
			{
				m_current_sprite = next;
			}
		}

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
		if(get_current_sprite_name().find(L"charging") == std::wstring::npos)
		{
			push_sprite(eCharacterAnimation::Charging, false);
		}

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

	void character::on_collision(const CollisionCode& collision, const Math::Vector2& hit_vector, const std::weak_ptr<Abstract::rigidBody>& other)
	{
		if(const auto ground = std::dynamic_pointer_cast<Object::Ground>(other.lock()))
		{
			const auto left_local_position = ground->to_top_left_inverse_local_position(get_left());
			const auto right_local_position = ground->to_top_left_local_position(get_right());

			// @todo: delta checks valid but this collision starts from right side it has the negative values, and considered as oob.
			// check should be done in reverse. use the positive side only is probably the best way for avoiding any problem.
			const Object::GroundState left_check = ground->safe_is_destroyed(left_local_position);
			const Object::GroundState right_check = ground->safe_is_destroyed(right_local_position);
			const auto orthogonal_surface = ground->safe_orthogonal_surface_global(get_bottom());

			// This ground is orthogonal surface. it will be treated as "main" ground.
			if(orthogonal_surface != Math::vector_inf)
			{
				ground_walk(collision, ground);
				ground_gravity(ground);
				ground_pitching(ground);
			}
			else
			{
				// If this ground is not orthogonal surface, probably this will exists in left or right, top side.
				// not-main ground is low enough to pass, then move it to the not-main ground.
				const Math::Vector2 bottom_local_position = ground->to_top_left_local_position(get_bottom());
				const auto possible_velocity = get_next_velocity(bottom_local_position, ground);

				if(possible_velocity != Math::vector_inf)
				{
					m_velocity = possible_velocity;
					ground_pitching(ground);
				}
				else if((left_check == Object::GroundState::NotDestroyed && get_velocity_offset() == Math::left) || 
					(right_check == Object::GroundState::NotDestroyed && get_velocity_offset() == Math::right))
				{
					m_velocity = {};
				}
			}
		}

		rigidBody::on_collision(collision, hit_vector, other);
	}

	void character::on_nocollison()
	{
		enable_gravity();
		m_bGrounded = false;
	}

	eCharacterState character::get_state() const
	{
		return m_state;
	}

	void character::default_state()
	{
		if(m_hp <= 0.0f)
		{
			m_state = eCharacterState::Dead;
		}
	}

	void character::idle_state()
	{
		if(m_bMovable)
		{
			default_state();

			m_power = 1.0f;

			const eKeyCode left_key = m_player_id == 0 ? eKeyCode::A : eKeyCode::LEFT;
			const eKeyCode right_key = m_player_id == 0 ? eKeyCode::D : eKeyCode::RIGHT;
			const eKeyCode up_key = m_player_id == 0 ? eKeyCode::W : eKeyCode::UP;
			const eKeyCode down_key = m_player_id == 0 ? eKeyCode::S : eKeyCode::DOWN;
			const eKeyCode firing_key = m_player_id == 0 ? eKeyCode::SPACE : eKeyCode::ENTER;
			const eKeyCode swap_key = m_player_id == 0 ? eKeyCode::TAB : eKeyCode::SHIFT;

			if (Input::getKey(up_key))
			{
				set_state(eCharacterState::Move);
				move_up();
			}
			else if (Input::getKey(left_key))
			{
				set_state(eCharacterState::Move);
				move_left();
			}
			else if (Input::getKey(down_key))
			{
				set_state(eCharacterState::Move);
				move_down();
			}
			else if (Input::getKey(right_key))
			{
				set_state(eCharacterState::Move);
				move_right();
			}
			else if (Input::getKey(firing_key))
			{
				set_state(eCharacterState::Firing);
				firing();
			}
			else if (Input::getKeyDown(swap_key) && !m_active_item.lock())
			{
				change_projectile();
			}
			else if (Input::getKeyDown(eKeyCode::One))
			{
				set_item_active(1);
			}
			else if (Input::getKeyDown(eKeyCode::Two))
			{
				set_item_active(2);
			}
			else if (Input::getKeyDown(eKeyCode::Three))
			{
				set_item_active(3);
			}
			else if (Input::getKeyUp(left_key) || Input::getKeyUp(right_key) || Input::getKeyUp(up_key) || Input::getKeyUp(down_key))
			{
				set_state(eCharacterState::Idle);
				stop();
			}
		}
	}

	void character::move_state()
	{
		if(m_bMovable)
		{
			default_state();
			static bool is_initial = true;

			if(is_initial)
			{
				m_sound_pack.get_sound(L"move").lock()->play(true);
				is_initial = false;
			}

			const eKeyCode left = m_player_id == 0 ? eKeyCode::A : eKeyCode::LEFT;
			const eKeyCode right = m_player_id == 0 ? eKeyCode::D : eKeyCode::RIGHT;
			const eKeyCode up = m_player_id == 0 ? eKeyCode::W : eKeyCode::UP;
			const eKeyCode down = m_player_id == 0 ? eKeyCode::S : eKeyCode::DOWN;

			if (Input::getKey(up))
			{
				set_state(eCharacterState::Move);
				move_up();
			}
			else if (Input::getKey(left))
			{
				set_state(eCharacterState::Move);
				move_left();
			}
			else if (Input::getKey(down))
			{
				set_state(eCharacterState::Move);
				move_down();
			}
			else if (Input::getKey(right))
			{
				set_state(eCharacterState::Move);
				move_right();
			}
			else if (Input::getKeyUp(left) || Input::getKeyUp(right) || Input::getKeyUp(up) || Input::getKeyUp(down))
			{
				m_sound_pack.get_sound(L"move").lock()->stop(true);
				is_initial = true;
				set_state(eCharacterState::Idle);
				stop();
			}
		}
	}

	void character::firing_state()
	{
		if(m_bMovable)
		{
			default_state();

			const eKeyCode firing_key = m_player_id == 0 ? eKeyCode::SPACE : eKeyCode::ENTER;
			if (Input::getKey(firing_key))
			{
				set_state(eCharacterState::Firing);
				firing();
			}
			if(Input::getKeyUp(firing_key))
			{
				if(const auto item = m_active_item.lock())
				{
					set_state(eCharacterState::Item);
				}
				else
				{
					set_state(eCharacterState::Fire);
					shoot();	
				}
			}
		}
	}

	void character::fire_state()
	{
		default_state();

		if(const auto prj = m_current_projectile.lock())
		{
			if(!prj->is_active())
			{
				set_state(eCharacterState::Idle);
			}
		}
	}

	void character::item_state()
	{
		if(const auto item = m_active_item.lock())
		{
			if(item->is_effect_ended())
			{
				set_state(eCharacterState::Idle);
				item->reset();
				m_active_item = {};
			}
			else
			{
				item->update(std::dynamic_pointer_cast<character>(shared_from_this()));	
			}
		}
	}

	void character::dead_state()
	{
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

	void character::set_item_active(const int n)
	{
		// item is applied to character only one.
		if(m_active_item.lock())
		{
			return;
		}

		if(m_available_items.find(n) != m_available_items.end())
		{
			push_sprite(eCharacterAnimation::Item, true);
			push_sprite(eCharacterAnimation::Idle, false);

			m_active_item = m_available_items[n];

			if(const auto item = m_active_item.lock())
			{
				if(item->is_instant())
				{
					set_state(eCharacterState::Item);
				}
			} 
			
		}
	}

	void character::push_sprite(const eCharacterAnimation& animation, bool override)
	{
		const std::wstring offset = get_offset() == Math::left ? L"left" : L"right";
		const std::wstring anim_name = get_animation_name(animation);
		const auto anim = m_texture.get_image(anim_name, offset);

		if(!m_current_sprite.lock())
		{
			m_current_sprite = anim;
			return;
		}

		if (override)
		{
			m_animation_queue.push(-1, anim);
			return;
		}

		m_animation_queue.push(static_cast<int>(animation), anim);
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
		push_sprite(eCharacterAnimation::Move, false);
		rigidBody::move_left();
	}

	void character::move_right()
	{
		push_sprite(eCharacterAnimation::Move, false);
		rigidBody::move_right();
	}

	void character::stop()
	{
		push_sprite(eCharacterAnimation::Idle, false);
		if(const auto move_sound = m_sound_pack.get_sound(L"move").lock())
		{
			if(move_sound->is_playing())
			{
				move_sound->stop(true);
			}
		}
		rigidBody::stop();
	}
	void character::prerender()
	{
	}
}
