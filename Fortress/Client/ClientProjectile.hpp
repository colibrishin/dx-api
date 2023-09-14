#pragma once
#include "CharacterProperties.hpp"
#include "../Common/projectile.hpp"
#include "../Common/debug.hpp"

namespace Fortress::Network::Client::Object
{
	class ClientProjectile : public ObjectBase::projectile
	{
	public:
		ClientProjectile() = delete;
		~ClientProjectile() override = default;
		void update() override;

	private:
		void send_fire() const;
		void send_flying() const;
		void send_ground_hit() const;
		void send_character_hit() const;

		void update_local_player() const;
		void update_non_local_player();

		bool has_state_changed() const;

		void notify_ground_hit() override;
		bool notify_character_hit() override;

		void fire() override;

	protected:
		ClientProjectile(const unsigned int id, const ObjectBase::character* shooter, const std::wstring& name, const std::wstring& short_name,
			const Math::Vector2& position, const Math::Vector2& velocity, const float mass, const Math::Vector2& speed,
			const Math::Vector2& acceleration, const float damage, const float radius, const int hit_count, const int fire_count,
			const float armor_penetration)
			: projectile(
				  id, shooter, name, short_name, position, velocity,
				  Fortress::Object::Property::projectile_hitbox_getter(), mass, speed, acceleration, damage, radius,
				  hit_count, fire_count, armor_penetration),
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
		EngineHandle::get_messenger()->send_message<ProjectileFlyingMsg>(
			eMessageType::ProjectileFlying, ProjectileFlyingMsg
			{{{}, eObjectType::Projectile, get_center(), get_offset()},
			get_id(), get_type()});
	}

	inline void ClientProjectile::send_ground_hit() const
	{
		EngineHandle::get_messenger()->send_message<ProjectileHitMsg>(
			eMessageType::ProjectileHit, ProjectileHitMsg
			{{{}, eObjectType::Projectile, get_center(), get_offset()},
			eObjectType::Ground, get_id(), get_type()});
	}

	inline void ClientProjectile::send_character_hit() const
	{
		EngineHandle::get_messenger()->send_message<ProjectileHitMsg>(
			eMessageType::ProjectileHit, ProjectileHitMsg
			{{{}, eObjectType::Projectile, get_center(), get_offset()},
			eObjectType::Character, get_id(), get_type()});
	}

	inline void ClientProjectile::send_fire() const
	{
		EngineHandle::get_messenger()->send_message<ProjectileFireMsg>(
			eMessageType::ProjectileFire, ProjectileFireMsg
			{{{}, eObjectType::Projectile, get_center(), get_offset()},
			get_id(), get_type()});
	}

	inline void ClientProjectile::update_local_player() const
	{
		if (get_origin()->is_localplayer())
		{
			static float tick_counter = 0.0f;

			if(get_state() == eProjectileState::Flying)
			{
				if(tick_counter > tick_rate)
				{
					send_flying();
					tick_counter = 0.0f;
				}
			}

			tick_counter += DeltaTime::get_deltaTime();
		}
	}

	inline void ClientProjectile::update_non_local_player()
	{
		if(!get_origin()->is_localplayer())
		{
			ProjectileFireMsg fire{};
			ProjectileFlyingMsg flying{};

			if(EngineHandle::get_messenger()->pop_message<ProjectileFireMsg>(
				eMessageType::ProjectileFire, get_origin()->get_player_id(), &fire, [&](const ProjectileFireMsg* msg)
			{
					return msg->prj_type == get_type() && msg->prj_id == get_id();
			}))
			{
				m_position = fire.position;
				set_offset(fire.offset);
				set_state(eProjectileState::Fire);
			}
			if(EngineHandle::get_messenger()->pop_message<ProjectileFlyingMsg>(
				eMessageType::ProjectileFlying, get_origin()->get_player_id(), &flying, [&](const ProjectileFlyingMsg* msg)
			{
					return msg->prj_type == get_type() && msg->prj_id == get_id();
			}))
			{
				m_position = flying.position;
				set_offset(flying.offset);
				set_state(eProjectileState::Flying);
			}
			if(EngineHandle::get_messenger()->pop_message<ProjectileHitMsg>(
				eMessageType::ProjectileHit, get_origin()->get_player_id(), &m_hit_msg_, [&](const ProjectileHitMsg* msg)
			{
					return msg->prj_type == get_type() && msg->prj_id == get_id() && msg->obj_type == eObjectType::Ground;
			}))
			{
				m_position = m_hit_msg_.position;
				set_offset(m_hit_msg_.offset);
				projectile::notify_ground_hit();
			}
			if(EngineHandle::get_messenger()->pop_message<ProjectileHitMsg>(
				eMessageType::ProjectileHit, get_origin()->get_player_id(), &m_hit_msg_, [&](const ProjectileHitMsg* msg)
			{
					return msg->prj_type == get_type() && msg->prj_id == get_id() && msg->obj_type == eObjectType::Character;
			}))
			{
				m_position = m_hit_msg_.position;
				set_offset(m_hit_msg_.offset);
				projectile::notify_character_hit();
			}
		}
	}

	inline bool ClientProjectile::has_state_changed() const
	{
		return m_previous_state_ != m_current_state_;
	}

	inline void ClientProjectile::notify_ground_hit()
	{
		projectile::notify_ground_hit();

		if(get_origin()->is_localplayer())
		{
			send_ground_hit();
		}
	}

	inline bool ClientProjectile::notify_character_hit()
	{
		if(projectile::notify_character_hit())
		{
			if(get_origin()->is_localplayer())
			{
				send_character_hit();
			}
			return true;
		}

		return false; 
	}

	inline void ClientProjectile::fire()
	{
		projectile::fire();

		if(get_origin()->is_localplayer())
		{
			send_fire();
		}
	}
}
