#include "character.hpp"

#include "deltatime.hpp"
#include "ground.hpp"
#include "input.hpp"
#include "projectile.hpp"

namespace Fortress::ObjectBase
{
	void character::initialize()
	{
		set_current_sprite(L"idle");
		m_current_projectile = m_main_projectile;
		rigidBody::initialize();
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
			set_current_sprite(L"hit");
			m_current_sprite.lock()->play([this]()
			{
				set_current_sprite(L"idle");
			});
		}
		else
		{
			set_current_sprite(L"death");
			m_current_sprite.lock()->play([this]()
			{
				set_current_sprite(L"dead");
				m_current_sprite.lock()->play();
			});

			set_state(eCharacterState::Dead);
		}
	}

	void character::shoot()
	{
		set_current_sprite(L"fire");

		// @todo: maybe queue?
		m_current_sprite.lock()->play([this]()
		{
			set_current_sprite(L"idle");
		});
		m_power = 1.0f;
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

	void character::get_next_position(
		const Math::Vector2& local_position_bottom,
		const std::weak_ptr<Object::Ground>& ground_ptr)
	{
		bool angle_check = false;
		bool climable = false;

		if (const auto ground = ground_ptr.lock())
		{
			// check up-hilling condition
			for(int i = 0; i < 10; i++)
			{
				if (angle_check && climable) 
				{
					break;
				}

				angle_check = false;

				for(int j = 99; j >= 0; --j)
				{
					Math::Vector2 local_new_pos = {
						local_position_bottom.get_x() + 
							(get_offset() == Math::left ?  -i : i),
						local_position_bottom.get_y() - j
					};

					const auto diff = local_new_pos - local_position_bottom;
					const auto unit = diff.normalized();
					const auto ground_check = ground->safe_is_destroyed(local_new_pos);

					if (isnan(unit.get_x()) || isnan(unit.get_y()))
					{
						continue;
					}

					if (ground_check == Object::GroundState::OutOfBound) 
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
								break;
							}
							
							climable = true;
							angle_check = true;
						}
						else 
						{
							m_velocity = unit;
						}
					}
				}
			}

			if(climable)
			{
				return;
			}

			for(int i = 0; i < 10; i++)
			{
				for(int j = 0; j < 100; ++j)
				{
					Math::Vector2 local_new_pos = {
						local_position_bottom.get_x() + 
							(get_offset() == Math::left ?  -i : i),
						local_position_bottom.get_y() + j
					};

					const auto diff = local_new_pos - local_position_bottom;
					const auto unit = diff.normalized();
					const auto ground_check = ground->safe_is_destroyed(local_new_pos);

					if (isnan(unit.get_x()) || isnan(unit.get_y()))
					{
						continue;
					}

					if (ground_check == Object::GroundState::NotDestroyed)
					{
						m_velocity = unit;
						return;
					}
				}
			}
		}

		if (!angle_check || !climable) 
		{
			m_velocity = {};
		}
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
			set_current_sprite(L"charging");
		}

		if(get_charged_power() < character_max_charge)
		{
			m_power += 100.0f * DeltaTime::get_deltaTime();
		}
		
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

	void character::on_collision(const CollisionCode& collision, const Math::Vector2& hit_vector, const std::weak_ptr<Abstract::rigidBody>& other)
	{
		if(const auto ground = std::dynamic_pointer_cast<Object::Ground>(other.lock()))
		{
			const Math::Vector2 bottom_local_position = ground->to_top_left_local_position(get_bottom());
			const Math::Vector2 left_local_position = ground->to_top_left_local_position(get_left());
			const Math::Vector2 right_local_position = ground->to_top_left_local_position(get_right());

			const Object::GroundState bottom_check = ground->safe_is_destroyed(bottom_local_position);
			// @todo: delta checks valid but this collision starts from right side it has the negative values, and considered as oob.
			// check should be done in reverse. use the positive side only is probably the best way for avoiding any problem.
			const Object::GroundState left_check = ground->safe_is_destroyed(left_local_position + ground->m_hitbox);
			const Object::GroundState right_check = ground->safe_is_destroyed(right_local_position);

			Debug::Log(get_name() + L" " + ground->get_name() + std::to_wstring(left_check == Object::GroundState::NotDestroyed));
			Debug::Log(get_name() + L" "  + ground->get_name() + std::to_wstring(right_check == Object::GroundState::NotDestroyed));

			if(collision == CollisionCode::Boundary)
			{
				if((get_velocity_offset() == Math::left && 
					left_check == Object::GroundState::NotDestroyed) ||
					(get_velocity_offset() == Math::right && 
					right_check == Object::GroundState::NotDestroyed))
				{
					// @todo: fixed animation
					// @todo: this might block the up/downhilling.
					m_velocity = {};
					return;
				}
			}

			if(collision == CollisionCode::Inside)
			{
				// @todo: climbing from lower side.
				if (bottom_check == Object::GroundState::NotDestroyed)
				{
					if (ground->safe_is_object_stuck_global(get_bottom())) 
					{
						const auto delta = ground->safe_nearest_surface(get_bottom());
						m_position -= delta;
					}
				}
				if(bottom_check == Object::GroundState::NotDestroyed &&
					m_state == eCharacterState::Move &&
					m_bGrounded)
				{
					get_next_position(bottom_local_position, ground);
				}
			}

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
				// @todo: reroute velocity to nearest ground point
				Debug::Log(L"Character hits the destroyed ground");
			}

			auto next_surface = get_offset() == Math::left ? get_bottom_left() : get_bottom_right();
			const auto delta = ground->safe_orthogonal_surface(next_surface, get_offset());
			next_surface += delta;

			auto rotate_radian = next_surface.local_inner_angle(get_bottom());
			const bool is_surface_lower = next_surface.get_y() > get_bottom().get_y();

			if((is_surface_lower && get_offset() == Math::left) ||
				(!is_surface_lower && get_offset() == Math::left))
			{
				rotate_radian = -rotate_radian;
			}

			if(get_offset() == Math::left)
			{
				rotate_radian = -rotate_radian;
			}
			
			set_movement_pitch_radian(rotate_radian / 2);
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
			else if (Input::getKeyDown(swap_key))
			{
				change_projectile();
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
				set_state(eCharacterState::Fire);
				shoot();
			}
		}
	}

	void character::fire_state()
	{
		default_state();

		static bool is_initial = true;

		if(is_initial)
		{
			// @todo: main/sub consideration
			m_current_projectile.lock()->play_fire_sound();
			is_initial = false;
		}

		if(const auto prj = m_current_projectile.lock())
		{
			if(!prj->is_active())
			{
				is_initial = true;
				set_state(eCharacterState::Idle);
			}
		}
	}

	void character::dead_state()
	{
	}

	void character::set_unmovable()
	{
		m_bMovable = false;
	}

	void character::set_movable()
	{
		m_bMovable = true;
	}

	void character::reset_mp()
	{
		m_mp = character_full_mp;
	}

	void character::set_current_sprite(const std::wstring& name)
	{
		m_current_sprite = m_texture.get_image(name, get_offset() == Math::left ? L"left" : L"right");
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
		set_current_sprite(L"move");
		rigidBody::move_left();
	}

	void character::move_right()
	{
		set_current_sprite(L"move");
		rigidBody::move_right();
	}

	void character::stop()
	{
		set_current_sprite(L"idle");
		rigidBody::stop();
	}
	void character::prerender()
	{
	}
}
