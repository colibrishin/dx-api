#pragma once
#ifndef CANNONCHARACTER_HPP
#define CANNONCHARACTER_HPP
#include "character.hpp"

namespace Fortress::Object
{
	class CannonCharacter final : public ObjectBase::character
	{
	public:
		CannonCharacter(const std::wstring& name, const Math::Vector2& position)
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

		~CannonCharacter() override
		{
			character::~character();
		};

		void initialize() override
		{
		}

		void shoot() override;
		void render() override;

	};
}

namespace Fortress::Object
{
	inline void CannonCharacter::shoot()
	{
		character::shoot();

		// refreshing the projectile position
		//m_base_projectile.m_position = {m_position.get_x() + m_hitbox.get_x() + 10.0f, m_position.get_y() - 10.0f};
		// set active for being calculated by rigidBody.
		//m_base_projectile.m_bActive = true;
	}

	inline void CannonCharacter::render()
	{
		character::render();

		if (is_active())
		{
			Ellipse(
				WinAPIHandles::get_buffer_dc(),
				get_x(),
				get_y(),
				get_x() + m_hitbox.get_x(),
				get_y() + m_hitbox.get_y());
		}
	}
}

#endif // CANNONCHARACTER_HPP
