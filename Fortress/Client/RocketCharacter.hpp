#pragma once
#ifndef CANNONCHARACTER_HPP
#define CANNONCHARACTER_HPP
#include "character.hpp"
#include "GifWrapper.hpp"
#include "math.h"
#include "resourceManager.hpp"

namespace Fortress::Object
{
	class RocketCharacter final : public ObjectBase::character
	{
	public:
		RocketCharacter(const std::wstring& name, const Math::Vector2& position)
			: character(
				name,
				position,
				{0.0f, 0.0f},
				{30.0f, 30.0f},
				500.0f,
				0.0f,
				ObjectBase::character_full_hp,
				ObjectBase::character_full_mp)
		{
			initialize();
		}

		~RocketCharacter() override
		{
			character::~character();
		};

		void initialize() override
		{
			m_idle_l = Resource::ResourceManager::load<GifWrapper>(
				L"Rocket Character Idle Left", "./resources/images/missile_idle_l.gif");

			m_idle_r = Resource::ResourceManager::load<GifWrapper>(
				L"Rocket Character Idle Right", "./resources/images/missile_idle_r.gif");

			m_hitbox = m_idle_l->get_hitbox();
			m_facing = {-1.0f, 0.0f};
			m_idle_l->load();
			m_idle_r->load();

			m_idle_l->play();
			m_idle_r->play();

			m_current_sprite = m_idle_l;
		}

		void shoot() override;
		void render() override;
		void move_left() override;
		void move_right() override;
	private:
		Math::Vector2 m_facing;

		GifWrapper* m_current_sprite;

		GifWrapper* m_idle_l;
		GifWrapper* m_idle_r;
	};
}

namespace Fortress::Object
{
	inline void RocketCharacter::shoot()
	{
		character::shoot();

		// refreshing the projectile position
		//m_base_projectile.m_position = {m_position.get_x() + m_hitbox.get_x() + 10.0f, m_position.get_y() - 10.0f};
		// set active for being calculated by rigidBody.
		//m_base_projectile.m_bActive = true;
	}

	inline void RocketCharacter::render()
	{
		character::render();

		if (is_active())
		{
			m_current_sprite->render(m_position);
		}
	}

	inline void RocketCharacter::move_left()
	{
		m_facing = Math::left;
		m_current_sprite = m_idle_l;
		character::move_left();
	}

	inline void RocketCharacter::move_right()
	{
		m_facing = Math::right;
		m_current_sprite = m_idle_r;
		character::move_right();
	}
}

#endif // CANNONCHARACTER_HPP
