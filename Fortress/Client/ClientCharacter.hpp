#pragma once
#include "NutshellProjectile.hpp"
#include "../Common/character.hpp"
#include "../Common/item.hpp"

namespace Fortress::Network::Client::Object
{
	class ClientCharacter : public ObjectBase::character
	{
	public:
		ClientCharacter() = delete;

		void update() override;

	private:
		void send_move() const;
		void send_stop() const;
		void send_firing() const;
		void send_fire() const;
		void send_pre_item() const;
		void send_item() const;

		void change_projectile() override;
		void equip_nutshell() override;
		void unequip_nutshell() override;

		void update_local_player() const;
		void update_non_local_player();

		bool has_state_changed() const;

	protected:
		ClientCharacter(const Network::PlayerID& player_id, const std::wstring& name, const std::wstring& short_name,
			const UnitVector& offset, const GlobalPosition& position, const UnitVector& velocity, float mass,
			const SpeedVector& speed, const AccelVector& acceleration, int hp, int mp, float armor)
			: character(
				  player_id, name, short_name, offset, position, velocity, mass, speed, acceleration, hp, mp, armor),
			  m_previous_state_(eCharacterState::Idle),
			  m_current_state_(eCharacterState::Idle)
		{
		}

		eCharacterState m_previous_state_;
		eCharacterState m_current_state_;

		std::weak_ptr<ObjectBase::projectile> get_nutshell_projectile() override;
	};

	inline void ClientCharacter::update()
	{
		m_previous_state_ = get_state();
		character::update();
		m_current_state_ = get_state();

		update_local_player();
		update_non_local_player();
	}

	inline void ClientCharacter::send_move() const
	{
		EngineHandle::get_messenger()->send_move_signal(get_position(), get_offset());
	}

	inline void ClientCharacter::send_stop() const
	{
		EngineHandle::get_messenger()->send_stop_signal(get_position(), get_offset());
	}

	inline void ClientCharacter::send_firing() const
	{
		EngineHandle::get_messenger()->send_firing_signal(get_position(), get_offset());
	}

	inline void ClientCharacter::send_fire() const
	{
		EngineHandle::get_messenger()->send_fire_signal(
			get_position(), 
			get_offset(), 
			get_charged_power());
	}

	inline void ClientCharacter::send_pre_item() const
	{
		EngineHandle::get_messenger()->send_item_signal(
						get_position(), get_offset(), m_active_item_index, m_active_item.lock()->get_item_type());
	}

	inline void ClientCharacter::send_item() const
	{
		EngineHandle::get_messenger()->send_item_fire_signal(
						get_position(), get_offset(), m_active_item_index, m_active_item.lock()->get_item_type(), get_charged_power());
	}

	inline void ClientCharacter::change_projectile()
	{
		character::change_projectile();

		if (is_movable_localplayer())
		{
			EngineHandle::get_messenger()->send_projectile_select_signal(get_projectile_type());
		}
	}

	inline void ClientCharacter::equip_nutshell()
	{
		character::equip_nutshell();

		if (is_movable_localplayer())
		{
			EngineHandle::get_messenger()->send_projectile_select_signal(get_projectile_type());
		}
	}

	inline void ClientCharacter::unequip_nutshell()
	{
		character::unequip_nutshell();

		if (is_movable_localplayer())
		{
			EngineHandle::get_messenger()->send_projectile_select_signal(get_projectile_type());
		}
	}

	inline void ClientCharacter::update_local_player() const
	{
		if (is_movable_localplayer() && has_state_changed())
		{
			switch (m_current_state_)
			{
			case eCharacterState::Idle:
			case eCharacterState::IdleLow:
				send_stop();
				break;
			case eCharacterState::Move:
			case eCharacterState::MoveLow:
				send_move();
				break;
			case eCharacterState::Firing:
				send_firing();
				break;
			case eCharacterState::FireSub:
			case eCharacterState::Fire:
				send_fire();
				break;
			case eCharacterState::Fired: break;
			case eCharacterState::PreItem: 
				send_pre_item();
				break;
			case eCharacterState::Item:
				send_item();
				break;
			case eCharacterState::TurnEnd: break;
			case eCharacterState::Dead: break;
			case eCharacterState::Death: break;
			case eCharacterState::Hit: break;
			default: ;
			}
		}
	}

	inline void ClientCharacter::update_non_local_player()
	{
		if(!is_localplayer())
		{
			PositionMsg position_msg{};
			FiringMsg firing_msg{};
			StopMsg stop_msg{};
			ProjectileSelectMsg projectile_msg{};
			ItemMsg item_msg{};
			FireMsg fire_msg{};
			ItemFireMsg item_fire_msg{};

			if (EngineHandle::get_messenger()->get_move_signal(get_player_id(), &position_msg))
			{
				set_offset(position_msg.offset);
				set_state(eCharacterState::Move);

				if (get_offset() == Math::left)
				{
					move_left();
				}
				else if (get_offset() == Math::right)
				{
					move_right();
				}
			}
			else if (EngineHandle::get_messenger()->get_stop_signal(get_player_id(), &stop_msg))
			{
				m_position = stop_msg.position;
				set_offset(stop_msg.offset);
				set_state(eCharacterState::Idle);
				stop();
			}
			else if (EngineHandle::get_messenger()->get_projectile_select_signal(get_player_id(), &projectile_msg))
			{
				m_projectile_type = projectile_msg.prj_type;
			}
			else if(EngineHandle::get_messenger()->get_item_signal(get_player_id(), &item_msg))
			{
				set_item_active(item_msg.index);
			}
			else if (EngineHandle::get_messenger()->get_firing_signal(get_player_id(), &firing_msg))
			{
				set_offset(firing_msg.offset);
				set_state(eCharacterState::Firing);
			}
			else if (EngineHandle::get_messenger()->get_fire_signal(get_player_id(), &fire_msg))
			{
				m_power = fire_msg.charged;
				set_state(eCharacterState::Fire);
				fire();
			}
			else if (m_active_item.lock() && EngineHandle::get_messenger()->get_item_fire_signal(
				get_player_id(), m_active_item.lock()->get_item_type(), &item_fire_msg))
			{
				m_power = item_fire_msg.charged;
				set_state(eCharacterState::Item);
			}
		}
	}

	inline bool ClientCharacter::has_state_changed() const
	{
		return m_previous_state_ != m_current_state_;
	}

	inline std::weak_ptr<ObjectBase::projectile> ClientCharacter::get_nutshell_projectile()
	{
		return ObjectBase::ObjectManager::create_object<Fortress::Object::NutShellProjectile>(this).lock();
	}
}
