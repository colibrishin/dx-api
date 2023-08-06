#include "character.hpp"

#include "deltatime.hpp"
#include "ground.hpp"
#include "NutshellProjectile.hpp"

namespace Fortress::ObjectBase
{
	void character::set_current_sprite(const eCharacterAnim& eAnim)
	{
		const auto offset = get_offset() == Math::left ? L"left" : L"right";
		const auto anim_name = anim_name_getter(eAnim);
		const auto anim = m_texture.get_image(anim_name, offset);
		const auto curr = m_current_sprite.lock();
		const auto next = anim.lock();

		if(curr != next)
		{
			m_current_sprite = next;
			next->stop();
			next->play();
			m_anim_elapsed = 0.0f;
		}
	}

	bool character::is_anim_finished() const
	{
		return m_anim_elapsed >= static_cast<float>(m_current_sprite.lock()->get_total_play_time()) / 1000.f;
	}

	void character::prerender()
	{
		m_anim_elapsed += DeltaTime::get_deltaTime();

		switch(get_state())
		{
		case eCharacterState::Fired:
		case eCharacterState::TurnEnd:
		case eCharacterState::Item:
		case eCharacterState::Idle: 
			if(is_anim_finished())
			{
				set_current_sprite(eCharacterAnim::Idle);
			}
			break;
		case eCharacterState::Move: 
			set_current_sprite(eCharacterAnim::Move);
			break;
		case eCharacterState::Firing: 
			set_current_sprite(eCharacterAnim::Firing);
			break;
		case eCharacterState::Fire:
			set_current_sprite(eCharacterAnim::Fire);
			break;
		case eCharacterState::PreItem:
			set_current_sprite(eCharacterAnim::Item);
			break;
		case eCharacterState::Dead:
			if(is_anim_finished())
			{
				set_current_sprite(eCharacterAnim::Dead);
			}
			break;
		case eCharacterState::Hit: 
			set_current_sprite(eCharacterAnim::Hit);
			break;
		case eCharacterState::Death: 
			set_current_sprite(eCharacterAnim::Death);
			break;
		default: ;
		}
	}
}
