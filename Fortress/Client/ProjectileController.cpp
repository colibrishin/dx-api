#include "ProjectileController.hpp"

#include "BattleScene.h"
#include "math.h"
#include "stateController.hpp"
#include "GifWrapper.h"

namespace Fortress::Controller
{
	ProjectileController::ProjectileController(
		const std::wstring& short_name, 
		const Math::Vector2& velocity,
		const Math::Vector2& fired_position,
		const Math::Vector2& position,
		const int max_hit_count,
		const int max_fire_count) :
		stateController(short_name, eProjectileState::Fire),
		m_pc_velocity(velocity),
		m_max_hit_count(max_hit_count),
		m_curr_hit_count(0),
		m_hit_cooldown(0.0f),
		m_fire_count(max_fire_count),
		m_bExploded(false),
		m_pitch(0.0f),
		m_fired_position(fired_position),
		m_pc_position(position),
		m_previous_position(fired_position)
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

		const auto unit = (m_pc_position - m_previous_position).normalized();
		m_pitch = std::fabs(unit.unit_angle());
		m_previous_position = m_pc_position;

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
			if(map->predicate_OOB(m_pc_position))
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
			hit();
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
		hit();

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
			m_pc_velocity * Math::Vector2{1, 0} == Math::left ? L"left" : L"right");
	}
}
