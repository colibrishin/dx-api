#pragma once
#include "../Common/projectile.hpp"

namespace Fortress::Network::Client::Object
{
	class ClientProjectile : public ObjectBase::projectile
	{
	public:
		ClientProjectile() = delete;
		void update() override;

	private:
		void send_flying() const;
		void send_ground_hit() const;
		void send_character_hit() const;
		
		void update_local_player() const;
		void update_non_local_player();

		bool has_state_changed() const;

	protected:
		ClientProjectile(const unsigned int id, const ObjectBase::character* shooter, const std::wstring& name, const std::wstring& short_name,
			const Math::Vector2& position, const Math::Vector2& velocity, float mass, const Math::Vector2& speed,
			const Math::Vector2& acceleration, float damage, float radius, int hit_count, int fire_count,
			float armor_penetration)
			: projectile(
				  id, shooter, name, short_name, position, velocity, mass, speed, acceleration, damage, radius, hit_count,
				  fire_count, armor_penetration),
				m_previous_state_(eProjectileState::Fire),
				m_current_state_(eProjectileState::Fire)
		{
		}

		eProjectileState m_previous_state_;
		eProjectileState m_current_state_;
	};

	inline void ClientProjectile::update()
	{
		m_previous_state_ = get_state();
		projectile::update();
		m_current_state_ = get_state();

		update_local_player();
		update_non_local_player();
	}

	inline void ClientProjectile::send_flying() const
	{
		EngineHandle::get_messenger()->send_
	}

	inline bool ClientProjectile::has_state_changed() const
	{
		return m_previous_state_ != m_current_state_;
	}

	inline void ClientProjectile::update_local_player() const
	{
		if (get_origin()->is_localplayer() && has_state_changed())
		{
			switch (m_current_state_)
			{
			case eProjectileState::Fire:
			case eProjectileState::Flying:
				send_flying();
				break;
			case eProjectileState::CharacterHit:
				send_character_hit();
				break;
			case eProjectileState::GroundHit:
				send_ground_hit();
				break;
			case eProjectileState::Destroyed: 
				break;
			default: ;
			}
		}
	}
}
