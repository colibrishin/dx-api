#pragma once
#ifndef CANNONCHARACTER_HPP
#define CANNONCHARACTER_HPP
#include "character.hpp"
#include "GifWrapper.hpp"
#include "math.h"
#include "resourceManager.hpp"

namespace Fortress::Object
{
	class MissileCharacter final : public ObjectBase::character
	{
	public:
		MissileCharacter(const std::wstring& name, const Math::Vector2& position)
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

		~MissileCharacter() override
		{
			character::~character();
		};

		void initialize() override
		{
			m_idle_l = Resource::ResourceManager::load<GifWrapper>(
				L"Rocket Character Idle Left", "./resources/images/missile_idle_l.gif");

			m_idle_r = Resource::ResourceManager::load<GifWrapper>(
				L"Rocket Character Idle Right", "./resources/images/missile_idle_r.gif");

			m_firing_l = Resource::ResourceManager::load<GifWrapper>(
				L"Rocket Character Firing Left", "./resources/images/missile_firing_l.gif");

			m_firing_r = Resource::ResourceManager::load<GifWrapper>(
				L"Rocket Character Firing Right", "./resources/images/missile_firing_r.gif");

			m_fire_l = Resource::ResourceManager::load<GifWrapper>(
				L"Rocket Character Fire Left", "./resources/images/missile_fire_l.gif");

			m_fire_r = Resource::ResourceManager::load<GifWrapper>(
				L"Rocket Character Fire Right", "./resources/images/missile_fire_r.gif");

			m_hitbox = m_idle_l->get_hitbox();
			m_facing = {-1.0f, 0.0f};

			// run silently.
			m_idle_l->play();
			m_idle_r->play();
			m_firing_l->play();
			m_firing_r->play();

			m_current_sprite = m_idle_l;
		}

		void firing() override;
		void shoot() override;
		void render() override;
		void move_left() override;
		void move_right() override;
	private:
		Math::Vector2 m_facing;

		GifWrapper* m_current_sprite;

		GifWrapper* m_idle_l;
		GifWrapper* m_idle_r;
		GifWrapper* m_firing_l;
		GifWrapper* m_firing_r;
		GifWrapper* m_fire_l;
		GifWrapper* m_fire_r;
	};
}

namespace Fortress::Object
{
	inline void MissileCharacter::firing()
	{
		if(m_current_sprite != m_firing_l || m_current_sprite != m_firing_r)
		{
			if(m_facing == Math::left)
			{
				m_current_sprite = m_firing_l;
			}
			else if(m_facing == Math::right)
			{
				m_current_sprite = m_firing_r;
			}
		}

		character::firing();
	}

	inline void MissileCharacter::shoot()
	{
		if(m_facing == Math::left)
		{
			m_current_sprite = m_fire_l;
		}
		else if(m_facing == Math::right)
		{
			m_current_sprite = m_fire_r;
		}

		// @todo: maybe queue?
		m_current_sprite->play([this]()
		{
			if(m_facing == Math::left)
			{
				m_current_sprite = m_idle_l;
			}
			else if(m_facing == Math::right)
			{
				m_current_sprite = m_idle_r;
			}
		});

		const float charged = get_charged_power();
		character::shoot();

		// refreshing the projectile position
		//m_base_projectile.m_position = {m_position.get_x() + m_hitbox.get_x() + 10.0f, m_position.get_y() - 10.0f};
		// set active for being calculated by rigidBody.
		//m_base_projectile.m_bActive = true;
	}

	inline void MissileCharacter::render()
	{
		character::render();

		if (is_active())
		{
			if(Scene::SceneManager::get_active_scene()->get_camera()->get_locked_object() == this)
			{
				m_current_sprite->render(Scene::SceneManager::get_active_scene()->get_camera()->get_offset());
			}
			else
			{
				m_current_sprite->render(m_position);
			}
		}
	}

	inline void MissileCharacter::move_left()
	{
		m_facing = Math::left;
		m_current_sprite = m_idle_l;
		character::move_left();
	}

	inline void MissileCharacter::move_right()
	{
		m_facing = Math::right;
		m_current_sprite = m_idle_r;
		character::move_right();
	}
}

#endif // CANNONCHARACTER_HPP
