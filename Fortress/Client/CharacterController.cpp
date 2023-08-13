#include "CharacterController.hpp"

#include "rigidbody.hpp"
#include "BattleScene.h"
#include "character.hpp"
#include "input.hpp"
#include "RepairItem.hpp"
#include "TeleportItem.hpp"
#include "DoubleShotItem.hpp"
#include "GifWrapper.h"

#undef max

namespace Fortress::Controller
{
	void CharacterController::initialize()
	{
		stateController::initialize();
		set_current_sprite(eCharacterState::Idle);
		set_state(eCharacterState::Idle);
		m_available_items.emplace(1, std::make_shared<Item::DoubleShotItem>());
		m_available_items.emplace(2, std::make_shared<Item::TeleportItem>());
		m_available_items.emplace(3, std::make_shared<Item::RepairItem>());
	}

	void CharacterController::update()
	{
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

	void CharacterController::render()
	{
		stateController::render();
	}

	void CharacterController::prerender()
	{
		stateController::prerender();

		switch(get_state())
		{
		case eCharacterState::Fired:
		case eCharacterState::TurnEnd:
		case eCharacterState::Item:
		case eCharacterState::Idle: 
			if(is_anim_finished())
			{
				set_current_sprite(eCharacterState::Idle);
			}
			break;
		case eCharacterState::Move: 
			set_current_sprite(eCharacterState::Move);
			break;
		case eCharacterState::Firing: 
			set_current_sprite(eCharacterState::Firing);
			break;
		case eCharacterState::Fire:
			set_current_sprite(eCharacterState::Fire);
			break;
		case eCharacterState::PreItem:
			set_current_sprite(eCharacterState::PreItem);
			break;
		case eCharacterState::Dead:
			if(is_anim_finished())
			{
				set_current_sprite(eCharacterState::Dead);
			}
			break;
		case eCharacterState::Hit: 
			set_current_sprite(eCharacterState::Hit);
			break;
		case eCharacterState::Death: 
			set_current_sprite(eCharacterState::Death);
			break;
		default: ;
		}
	}

	void CharacterController::set_unmovable()
	{
		m_bMovable = false;
		stop();
	}

	void CharacterController::set_movable()
	{
		m_bMovable = true;
	}

	bool CharacterController::is_projectile_fire_counted() const
	{
		int fire_count = 0;
		int exploded = 0;

		for(const auto& ptr : m_active_projectiles)
		{
			if(const auto prj = ptr.lock())
			{
				if(prj->get_state() == eProjectileState::Destroyed)
				{
					exploded++;
				}

				fire_count = std::max(fire_count, prj->get_fire_count());
			}
		}

		return fire_count == exploded;
	}

	bool CharacterController::is_projectile_active() const
	{
		if(m_active_projectiles.empty())
		{
			return false;
		}

		return std::any_of(
			m_active_projectiles.begin(), 
			m_active_projectiles.end(), 
			[](const ProjectilePointer& ptr)
			{
				if(const auto prj = ptr.lock())
				{
;					return prj->get_state() != eProjectileState::Destroyed;
				}

				return false;
			});
	}

	float CharacterController::get_charged_power() const
	{
		return m_power;
	}

	float CharacterController::get_hp_percentage() const
	{
		if(m_hp <= 0.0f)
		{
			return 0.0f;
		}

		return m_hp / static_cast<float>(ObjectBase::character_full_hp);
	}

	float CharacterController::get_mp_percentage() const
	{
		if(m_hp <= 0.0f)
		{
			return 0.0f;
		}

		return m_mp / static_cast<float>(ObjectBase::character_full_mp);
	}

	float CharacterController::get_hp_raw() const
	{
		return m_hp;
	}

	const std::vector<std::weak_ptr<ObjectBase::projectile>>& CharacterController::get_projectiles() const
	{
		return m_active_projectiles;
	}

	const eProjectileType& CharacterController::get_projectile_type() const
	{
		return m_projectile_type;
	}

	std::weak_ptr<ObjectBase::projectile> CharacterController::get_one_active_projectile()
	{
		if(m_active_projectiles.empty())
		{
			return {};
		}

		for(const auto& prj : m_active_projectiles)
		{
			if(const auto projectile = prj.lock())
			{
				if(projectile->is_active() && 
					projectile->get_origin() == dynamic_cast<ObjectBase::character*>(this))
				{
					return prj;
				}
			}
		}

		return {};
	}

	CharacterController::CharacterController(
		const std::wstring& short_name, 
		const float hp, 
		const float mp,
		RefOnlyRigidBodyPointer rb) :
		stateController(short_name, eCharacterState::Idle),
		m_hp(hp),
		m_mp(mp),
		m_power(1.0f),
		m_bMovable(false),
		m_rb(rb),
		m_projectile_type(eProjectileType::Main),
		m_tmp_projectile_type(eProjectileType::Main),
		m_active_item()
	{
		CharacterController::initialize();
	}

	void CharacterController::set_sprite_offset(const std::wstring& name, const std::wstring& orientation,
		const Math::Vector2& offset)
	{
		m_texture.get_image(name, orientation).lock()->set_offset(offset);
	}

	const std::wstring& CharacterController::get_current_sprite_name() const
	{
		return m_current_sprite.lock()->get_name();
	}

	void CharacterController::fire()
	{
	}

	void CharacterController::move_left()
	{
		if(const auto move_sound = m_sound_pack.get_sound(L"move").lock())
		{
			if(!move_sound->is_playing())
			{
				move_sound->play(true);
			}
		}

		set_state(eCharacterState::Move);
	}

	void CharacterController::move_right()
	{
		if(const auto move_sound = m_sound_pack.get_sound(L"move").lock())
		{
			if(!move_sound->is_playing())
			{
				move_sound->play(true);
			}
		}

		set_state(eCharacterState::Move);
	}

	void CharacterController::stop()
	{
		if(const auto move_sound = m_sound_pack.get_sound(L"move").lock())
		{
			if(move_sound->is_playing())
			{
				move_sound->stop(true);
			}
		}
	}

	void CharacterController::set_current_sprite(const eCharacterState& state)
	{
		const auto offset = m_rb->get_offset() == Math::left ? L"left" : L"right";
		const auto anim_name = anim_name_getter(state);
		const auto anim = m_texture.get_image(anim_name, offset);
		const auto curr = m_current_sprite.lock();

		if(const auto next = anim.lock(); curr != next)
		{
			m_current_sprite = next;
			next->stop();
			next->play();
			reset_anim_counter();
		}
	}

	std::wstring CharacterController::anim_name_getter(const eCharacterState& state)
	{
		switch(state)
		{
		case eCharacterState::Fired: 
		case eCharacterState::TurnEnd:
		case eCharacterState::Idle:
			return L"idle";
		case eCharacterState::Move: 
			return L"move";
		case eCharacterState::Firing: 
			return L"charging";
		case eCharacterState::Item:
		case eCharacterState::Fire:
			return L"fire";
		case eCharacterState::PreItem: 
			return L"item";
		case eCharacterState::Dead: 
			return L"dead";
		case eCharacterState::Death: 
			return L"death";
		case eCharacterState::Hit: 
			return L"hit";
		default: return L"";
		}
	}

	void CharacterController::default_state()
	{
		if(m_hp <= 0.0f)
		{
			set_state(eCharacterState::Death);
		}

		const auto map =  Scene::SceneManager::get_active_map().lock();

		if(map->predicate_OOB(m_rb->get_offset_bottom_forward_position()))
		{
			set_state(eCharacterState::Death);
		}
	}

	void CharacterController::idle_state()
	{
		if(m_bMovable)
		{
			default_state();

			m_power = 1.0f;

			if (Input::getKey(eKeyCode::A))
			{
				set_state(eCharacterState::Move);
				move_left();
			}
			else if (Input::getKey(eKeyCode::D))
			{
				set_state(eCharacterState::Move);
				move_right();
			}
			else if (Input::getKey(eKeyCode::SPACE))
			{
				set_state(eCharacterState::Firing);
			}
			else if (Input::getKeyDown(eKeyCode::TAB) && !m_active_item.lock())
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
			else if (Input::getKeyUp(eKeyCode::A) || Input::getKeyUp(eKeyCode::D))
			{
				set_state(eCharacterState::Idle);
				stop();
			}
		}
	}

	void CharacterController::move_state()
	{
		if(m_bMovable)
		{
			default_state();

			const auto map =  Scene::SceneManager::get_active_map().lock();

			if(!map->movable(m_rb->get_offset_bottom_forward_position()))
			{
				// @todo: fixed motion.
				set_state(eCharacterState::Idle);
			}

			if (Input::getKey(eKeyCode::A))
			{
				set_state(eCharacterState::Move);
				move_left();
			}
			else if (Input::getKey(eKeyCode::D))
			{
				set_state(eCharacterState::Move);
				move_right();
			}
			else if (Input::getKeyUp(eKeyCode::A) || Input::getKeyUp(eKeyCode::D))
			{
				set_state(eCharacterState::Idle);
				stop();
			}
		}
	}

	void CharacterController::firing_state()
	{
		if(m_bMovable)
		{
			default_state();

			if (m_power < ObjectBase::character_max_charge)
			{
				m_power += 100.0f * DeltaTime::get_deltaTime();
			}

			if (Input::getKey(eKeyCode::SPACE))
			{
				set_state(eCharacterState::Firing);
			}
			if (Input::getKeyUp(eKeyCode::SPACE))
			{
				if(const auto item = m_active_item.lock())
				{
					set_state(eCharacterState::Item);
				}
				else
				{
					set_state(eCharacterState::Fire);
					fire();
				}
			}
		}
	}

	void CharacterController::fire_state()
	{
		default_state();

		const auto scene = Scene::SceneManager::get_active_scene().lock();
		const auto projectile_list = scene->get_objects<ObjectBase::projectile>();

		if(projectile_list.empty())
		{
			return;
		}

		const auto initial_prj = (*projectile_list.begin()).lock();

		// @todo: too broad assumption
		if(initial_prj->get_origin() == dynamic_cast<ObjectBase::character*>(this) && 
			initial_prj->get_state() != eProjectileState::Fire)
		{
			set_state(eCharacterState::Fired);
		}
	}

	void CharacterController::fired_state()
	{
		if(is_projectile_fire_counted() && !is_projectile_active())
		{
			set_state(eCharacterState::TurnEnd);
		}
	}

	void CharacterController::hit_state()
	{
		default_state();

		if(const auto item = m_active_item.lock())
		{
			set_state(eCharacterState::Item);
			return;
		}

		if(!is_projectile_fire_counted() && is_projectile_active())
		{
			set_state(eCharacterState::Fired);
			return;
		}

		if(is_projectile_fire_counted() && !is_projectile_active())
		{
			set_state(eCharacterState::TurnEnd);
			return;
		}

		set_state(eCharacterState::Idle);
	}

	void CharacterController::turn_end_state()
	{
		default_state();

		const auto scene = Scene::SceneManager::get_active_scene().lock();
		const auto projectiles = get_projectiles();

		for(const auto& prj : projectiles)
		{
			scene->remove_game_object(Abstract::LayerType::Projectile, prj);
		}

		m_active_projectiles.clear();
		reset_mp();

		if(projectiles.empty())
		{
			set_state(eCharacterState::Idle);
		}
	}

	void CharacterController::preitem_state()
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

	void CharacterController::item_state()
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
				item->update(downcast_from_this<ObjectBase::character>());	
			}
		}
	}

	void CharacterController::death_state()
	{
		set_unmovable();
		set_state(eCharacterState::Dead);
	}

	void CharacterController::dead_state()
	{
	}

	void CharacterController::set_item_active(const int n)
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

	void CharacterController::change_projectile()
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

	void CharacterController::move()
	{
		if (m_mp < Math::epsilon)
		{
			return;
		}

		if (std::fabs(m_rb->get_velocity().get_x()) > Math::epsilon)
		{
			m_mp -= 20.0f * DeltaTime::get_deltaTime();
		}
	}

	void CharacterController::apply_damage(float damage)
	{
		m_hp -= damage;
		set_state(eCharacterState::Hit);
	}

	void CharacterController::set_hp(const float hp)
	{
		if(hp >= ObjectBase::character_full_hp)
		{
			m_hp = ObjectBase::character_full_hp;
		}
		else if(hp <= 0.0f)
		{
			m_hp = 0.0f;
		}
		else
		{
			m_hp = hp;
		}
	}

	void CharacterController::add_active_projectile(const std::weak_ptr<ObjectBase::projectile>& prj)
	{
		m_active_projectiles.push_back(prj);
	}

	void CharacterController::equip_nutshell()
	{
		m_tmp_projectile_type = m_projectile_type;
		m_projectile_type = eProjectileType::Nutshell;
	}

	void CharacterController::unequip_nutshell()
	{
		if(m_tmp_projectile_type != eProjectileType::Nutshell && m_projectile_type == eProjectileType::Nutshell)
		{
			m_projectile_type = m_tmp_projectile_type;
		}
	}

	void CharacterController::reset_mp()
	{
		m_mp = ObjectBase::character_full_mp;
	}
}
