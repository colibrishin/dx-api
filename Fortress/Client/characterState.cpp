#include "character.hpp"
#include "input.hpp"
#include "projectile.hpp"
#include "item.hpp"

#undef max

namespace Fortress::ObjectBase
{
	void character::update()
	{
		rigidBody::update();

		switch(get_state())
		{
		case eCharacterState::Idle:
			idle_state();
			break;
		case eCharacterState::Firing:
			firing_state();
			break;
		case eCharacterState::Fire:
			fire_state();
			break;
		case eCharacterState::Move:
			move_state();
			break;
		case eCharacterState::Dead:
			dead_state();
			break;
		case eCharacterState::Death: 
			death_state();
			break;
		case eCharacterState::Fired:
			fired_state();
			break;
		case eCharacterState::PreItem:
			preitem_state();
			break;
		case eCharacterState::Hit:
			hit_state();
			break;
		case eCharacterState::Item: 
			item_state();
			break;
		case eCharacterState::TurnEnd: 
			turn_end_state();
			break;
		default:
			break;
		}
	}

	void character::set_state(const eCharacterState& state)
	{
		m_state = state;
	}

	eCharacterState character::get_state() const
	{
		return m_state;
	}

	void character::default_state()
	{
		if(m_hp <= 0.0f)
		{
			set_state(eCharacterState::Death);
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

		const auto scene = Scene::SceneManager::get_active_scene().lock();
		const auto projectile_list = scene->get_objects<projectile>();

		if(projectile_list.empty())
		{
			return;
		}

		const auto initial_prj = (*projectile_list.begin()).lock();

		if(initial_prj->get_origin() == this && 
			(initial_prj->is_active() || initial_prj->is_exploded()))
		{
			set_state(eCharacterState::Fired);
		}
	}

	void character::fired_state()
	{
		if(!is_projectile_fire_counted() && !is_projectile_active())
		{
			set_state(eCharacterState::TurnEnd);
		}
	}

	void character::hit_state()
	{
		default_state();

		if(const auto item = m_active_item.lock())
		{
			set_state(eCharacterState::Item);
			return;
		}

		if(is_projectile_fire_counted())
		{
			set_state(eCharacterState::Fired);
			return;
		}

		set_state(eCharacterState::Idle);
	}

	void character::turn_end_state()
	{
		default_state();

		const auto scene = Scene::SceneManager::get_active_scene().lock();
		const auto projectiles = get_projectiles();

		for(const auto& prj : projectiles)
		{
			scene->remove_game_object(Abstract::LayerType::Projectile, prj);
		}

		set_state(eCharacterState::Idle);
	}

	void character::preitem_state()
	{
		default_state();

		if (const auto item = m_active_item.lock())
		{
			if (item->is_instant())
			{
				set_state(eCharacterState::Item);
				return;
			}
		}

		set_state(eCharacterState::Idle);
	}

	void character::item_state()
	{
		default_state();

		if(const auto item = m_active_item.lock())
		{
			if(item->is_effect_ended())
			{
				set_state(eCharacterState::TurnEnd);
				item->reset();
				m_active_item = {};
			}
			else
			{
				item->update(std::dynamic_pointer_cast<character>(shared_from_this()));	
			}
		}
	}

	void character::death_state()
	{
		set_state(eCharacterState::Dead);
	}

	void character::dead_state()
	{
	}

	void character::set_item_active(const int n)
	{
		if (m_bMovable)
		{
			// item is applied to character only one.
			if (m_active_item.lock())
			{
				return;
			}

			if (m_available_items.find(n) != m_available_items.end())
			{
				m_active_item = m_available_items[n];
				set_state(eCharacterState::PreItem);
			}
		}
	}
}
