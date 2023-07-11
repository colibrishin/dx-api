#pragma once
#ifndef CANNONCHARACTER_HPP
#define CANNONCHARACTER_HPP
#include "character.hpp"
#include "ImageWrapper.hpp"
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
			// @todo: add gif wrapper
			m_image = Resource::ResourceManager::load<ImageWrapper>(
				L"Rocket Character", "./resources/images/missile_idle.gif");
			m_hitbox = m_image->get_hitbox();
			m_facing = {-1.0f, 0.0f};
		}

		void shoot() override;
		void render() override;
		void move_left() override;
		void move_right() override;
	private:
		Math::Vector2 m_facing;
		ImageWrapper* m_image;
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
			if(m_facing != Math::Vector2{-1.0f, 0.0f})
			{
				m_image->flip();
			}

			m_image->render(m_position);

			if(m_facing != Math::Vector2{-1.0f, 0.0f})
			{
				m_image->flip();
			}
		}
	}

	inline void RocketCharacter::move_left()
	{
		m_facing = {-1.0f, 0.0f};
		character::move_left();
	}

	inline void RocketCharacter::move_right()
	{
		m_facing = {1.0f, 0.0f};
		character::move_right();
	}
}

#endif // CANNONCHARACTER_HPP
