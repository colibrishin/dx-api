#include "pch.h"
#include "ProjectileController.hpp"

#include "BattleScene.h"
#include "math.h"
#include "stateController.hpp"
#include "GifWrapper.h"

namespace Fortress::Controller
{
	ProjectileController::ProjectileController(
		const ObjectBase::character* shooter,
		const std::wstring& short_name, 
		const Abstract::rigidBody* const rb,
		const int max_hit_count,
		const int max_fire_count) :
		stateController(short_name, eProjectileState::Fire),
		m_shooter(shooter),
		m_rb(rb),
		m_max_hit_count(max_hit_count),
		m_curr_hit_count(0),
		m_hit_cooldown(0.0f),
		m_fire_count(max_fire_count),
		m_bExploded(false),
		m_pitch(0.0f),
		m_previous_position(rb->get_position()),
		m_hitmsg_()
	{
	}

	void ProjectileController::initialize()
	{
		stateController::initialize();
		set_current_sprite(eProjectileState::Fire);
		set_state(eProjectileState::Fire);
	}

	void ProjectileController::update()
	{
		m_hit_cooldown += DeltaTime::get_deltaTime();

		if(!m_ch->is_localplayer() && 
			(get_state() != eProjectileState::CharacterHit || 
				get_state() != eProjectileState::GroundHit))
		{
			if (EngineHandle::get_messenger()->get_hit_signal(m_ch->get_player_id(), &m_hitmsg_))
			{
				switch(m_hitmsg_.object_type)
				{
				case Network::eObjectType::Character:
					notify_character_hit();
					break;
				case Network::eObjectType::Projectile: break;
				case Network::eObjectType::Ground: 
					notify_ground_hit();
					break;
				default: ;
				}
			}
		}

		switch(get_state())
		{
		case eProjectileState::Fire: 
			fire_state();
			break;
		case eProjectileState::Flying: 
			flying_state();
			break;
		case eProjectileState::CharacterHit: 
			character_hit_state();
			break;
		case eProjectileState::GroundHit:
			ground_hit_state();
			break;
		case eProjectileState::Destroyed: 
			destroyed_state();
			break;
		default: ;
		}
	}

	void ProjectileController::prerender()
	{
		stateController::prerender();

		const auto diff_by_offset = get_moving_direction() == Math::left ?
			m_previous_position - m_rb->get_position() :
			m_rb->get_position() - m_previous_position;

		const auto unit = diff_by_offset.normalized();
		m_pitch = unit.unit_angle();

		switch(get_state())
		{
		case eProjectileState::Fire: 
		case eProjectileState::Flying: 
		case eProjectileState::CharacterHit:
		case eProjectileState::GroundHit:
		case eProjectileState::Destroyed:
			set_current_sprite(eProjectileState::Fire);
		default: ;
		}

		m_previous_position = m_rb->get_position();
	}

	bool ProjectileController::is_cooldown() const
	{
		if(m_curr_hit_count == 0)
		{
			return false;
		}

		return (m_hit_cooldown < 0.5f);
	}

	bool ProjectileController::is_exploded() const
	{
		return m_bExploded;
	}

	int ProjectileController::get_hit_count() const
	{
		return m_curr_hit_count;
	}

	int ProjectileController::get_fire_count() const
	{
		return m_fire_count;
	}

	int ProjectileController::get_max_hit_count() const
	{
		return m_max_hit_count;
	}

	const ObjectBase::character* ProjectileController::get_origin() const
	{
		return m_shooter;
	}

	void ProjectileController::fire_state()
	{
		fire();
		m_bExploded = false;
		m_current_sprite.lock()->play();
		play_fire_sound();

		set_state(eProjectileState::Flying);
	}

	void ProjectileController::flying_state()
	{
		flying();

		if (const auto map = Scene::SceneManager::get_active_map().lock())
		{
			if(map->predicate_OOB(m_rb->get_position()))
			{
				set_state(eProjectileState::Destroyed);
			}
		}
	}

	void ProjectileController::character_hit_state()
	{
		if(!is_cooldown())
		{
			increase_hit_count();
			reset_cooldown();
			play_hit_sound();
			if(m_ch->is_localplayer())
			{
				EngineHandle::get_messenger()->send_hit_signal(
					Network::eObjectType::Character, m_rb->get_center());
				hit();	
			}
			else
			{
				if (m_hitmsg_.object_type == Network::eObjectType::Character)
				{
					downcast_from_this<ObjectBase::projectile>()->m_position = m_hitmsg_.position;
					hit();
					m_hitmsg_ = {};
				}
			}
		}

		if(m_max_hit_count == m_curr_hit_count)
		{
			set_state(eProjectileState::Destroyed);
			return;
		}

		set_state(eProjectileState::Flying);
	}

	void ProjectileController::ground_hit_state()
	{
		increase_hit_count();
		reset_cooldown();
		play_hit_sound();
		if(m_ch->is_localplayer())
		{
			EngineHandle::get_messenger()->send_hit_signal(
					Network::eObjectType::Ground, m_rb->get_center());
			hit();
		}
		else
		{
			if(m_hitmsg_.object_type == Network::eObjectType::Ground)
			{
				downcast_from_this<ObjectBase::projectile>()->m_position = m_hitmsg_.position;
				hit();
				m_hitmsg_ = {};
			}
		}

		if(m_max_hit_count == m_curr_hit_count)
		{
			set_state(eProjectileState::Destroyed);
			return;
		}

		set_state(eProjectileState::Flying);
	}

	void ProjectileController::destroyed_state()
	{
		destroyed();
		m_curr_hit_count = 0;
		m_pitch = 0;
		m_bExploded = true;
		reset_cooldown();
		m_current_sprite.lock()->reset_transform();
	}

	void ProjectileController::reset_cooldown()
	{
		m_hit_cooldown = 0.0f;
	}

	void ProjectileController::increase_hit_count()
	{
		m_curr_hit_count++;
	}

	float ProjectileController::get_pitch() const
	{
		return m_pitch;
	}

	void ProjectileController::notify_character_hit()
	{
		if(!is_cooldown())
		{
			set_state(eProjectileState::CharacterHit);
		}
	}

	void ProjectileController::notify_ground_hit()
	{

		set_state(eProjectileState::GroundHit);
	}

	void ProjectileController::set_current_sprite(const eProjectileState& state)
	{
		m_current_sprite = m_texture.get_image(
			L"projectile",
			get_moving_direction() == Math::left ? L"left" : L"right");
	}

	DirVector ProjectileController::get_moving_direction() const
	{
		const UnitVector moving_dir = (m_rb->get_position() - m_previous_position).normalized();
		return moving_dir.x_dir();
	}
}
