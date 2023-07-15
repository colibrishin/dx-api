#pragma once
#ifndef CANNONCHARACTER_HPP
#define CANNONCHARACTER_HPP
#include "character.hpp"
#include "GifWrapper.hpp"
#include "math.h"
#include "MissileProjectile.hpp"
#include "resourceManager.hpp"

namespace Fortress::Object
{
	class MissileCharacter final : public ObjectBase::character
	{
	public:
		MissileCharacter(const std::wstring& name, const Math::Vector2& position, const Math::Vector2& orientation)
			: character(
				name,
				position,
				{0.0f, 0.0f},
				{46.0f, 53.0f},
				500.0f,
				0.0f,
				ObjectBase::character_full_hp,
				ObjectBase::character_full_mp),
				m_texture(L"missile"),
				m_offset(orientation)
		{
			initialize();
		}

		~MissileCharacter() override
		{
			character::~character();
		}

		void initialize() override
		{
			m_idle_l = m_texture.get_image(L"idle", L"left");
			m_idle_r = m_texture.get_image(L"idle", L"right");
			m_charging_l = m_texture.get_image(L"charging", L"left");
			m_charging_r = m_texture.get_image(L"charging", L"right");
			m_fire_l = m_texture.get_image(L"fire", L"left");
			m_fire_r = m_texture.get_image(L"fire", L"right");
			m_move_l = m_texture.get_image(L"move", L"left");
			m_move_r = m_texture.get_image(L"move", L"right");

			m_charging_l->set_offset({0, 10.0f});
			m_charging_r->set_offset({0, 10.0f});
			m_fire_l->set_offset({0, 10.0f});
			m_fire_r->set_offset({0, 10.0f});
			m_hitbox = m_idle_l->get_hitbox();

			// run silently.
			m_idle_l->play();
			m_idle_r->play();
			m_charging_l->play();
			m_charging_r->play();
			m_move_l->play();
			m_move_r->play();

			m_current_sprite = m_idle_l;
		}

		void firing() override;
		void shoot() override;
		void render() override;
		void move_left() override;
		void move_right() override;
		void stop() override;
	private:
		MissileProjectile m_base_projectile;

		Math::Vector2 m_offset;

		Texture<GifWrapper> m_texture;

		GifWrapper* m_current_sprite;

		GifWrapper* m_idle_l;
		GifWrapper* m_idle_r;
		GifWrapper* m_charging_l;
		GifWrapper* m_charging_r;
		GifWrapper* m_fire_l;
		GifWrapper* m_fire_r;
		GifWrapper* m_move_l;
		GifWrapper* m_move_r;
	};
}

namespace Fortress::Object
{
	inline void MissileCharacter::firing()
	{
		if(m_current_sprite != m_charging_l || m_current_sprite != m_charging_r)
		{
			if(m_offset == Math::left)
			{
				m_current_sprite = m_charging_l;
			}
			else if(m_offset == Math::right)
			{
				m_current_sprite = m_charging_r;
			}
		}

		character::firing();
	}

	inline void MissileCharacter::shoot()
	{
		if(m_offset == Math::left)
		{
			m_current_sprite = m_fire_l;
		}
		else if(m_offset == Math::right)
		{
			m_current_sprite = m_fire_r;
		}

		// @todo: maybe queue?
		m_current_sprite->play([this]()
		{
			if(m_offset == Math::left)
			{
				m_current_sprite = m_idle_l;
			}
			else if(m_offset == Math::right)
			{
				m_current_sprite = m_idle_r;
			}
		});

		float charged = get_charged_power();

		if(charged < 0)
		{
			charged = 10.0f;
		}

		character::shoot();

		m_base_projectile.fire(m_offset == Math::left ? get_top_left() : get_top_right(), m_offset, charged);
	}

	inline void MissileCharacter::render()
	{
		character::render();

		Math::Vector2 pos{};

		if (is_active())
		{
			if(Scene::SceneManager::get_active_scene()->get_camera()->get_locked_object() == this)
			{
				pos = Scene::SceneManager::get_active_scene()->get_camera()->get_offset();
			}
			else
			{
				pos = Scene::SceneManager::get_active_scene()->get_camera()->get_relative_position(this);
			}

			m_current_sprite->render(pos, m_hitbox);

			Debug::Log(L"Char pos" + std::to_wstring(pos.get_x()) + std::to_wstring(pos.get_y()));
			Debug::draw_rect(pos, m_hitbox);
			Debug::draw_dot(pos);
		}
	}

	inline void MissileCharacter::move_left()
	{
		m_offset = Math::left;
		m_current_sprite = m_move_l;
		character::move_left();
	}

	inline void MissileCharacter::move_right()
	{
		m_offset = Math::right;
		m_current_sprite = m_move_r;
		character::move_right();
	}

	inline void MissileCharacter::stop()
	{
		if(m_offset == Math::left)
		{
			m_current_sprite = m_idle_l;
		}
		else if(m_offset == Math::right)
		{
			m_current_sprite = m_idle_r;
		}
		character::stop();
	}
}

#endif // CANNONCHARACTER_HPP
