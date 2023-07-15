#pragma once
#ifndef PRECISONPROJECTILE_HPP
#define PRECISONPROJECTILE_HPP

#include "GifWrapper.hpp"
#include "projectile.hpp"
#include "math.h"
#include "resourceManager.hpp"
#include "Texture.hpp"

namespace Fortress::Object
{
	class MissileProjectile final : public ObjectBase::projectile
	{
	public:
		MissileProjectile() : projectile(L"Precision Projectile", {}, {10.0f, 10.0f}, Math::identity, 2.0f, 0.0f, 10.0f), m_texture(L"missile")
		{
			MissileProjectile::initialize();
		}
		~MissileProjectile() override
		{
			projectile::~projectile();
		}

		void initialize() override;
		void update() override;
		void render() override;
		void focus_this();
		void unfocus_this();
		void fire(const Math::Vector2& position, const Math::Vector2 velocity, const float charged);
		virtual void on_collision(rigidBody* other) override;

	private:
		Math::Vector2 m_fired_position;
		Texture<GifWrapper> m_texture;

		GifWrapper* m_current_sprite;

		GifWrapper* m_left{};
		GifWrapper* m_right{};
	};

	inline void MissileProjectile::initialize()
	{
		projectile::initialize();

		m_left = m_texture.get_image(L"projectile", L"left");
		m_right = m_texture.get_image(L"projectile", L"right");

		m_current_sprite = m_left;

		m_left->load();
		m_left->play(nullptr);
		m_right->load();
		m_right->play(nullptr);
	}

	inline void MissileProjectile::update()
	{
		if(m_position.get_y() >= WinAPIHandles::get_actual_max_y() - 50.0f)
		{
			unfocus_this();
		}

		projectile::update();
	}

	inline void MissileProjectile::render()
	{
		if(is_active())
		{
			projectile::render();

			Math::Vector2 pos{};

			if(Scene::SceneManager::get_active_scene()->get_camera()->get_locked_object() == this)
			{
				pos = Scene::SceneManager::get_active_scene()->get_camera()->get_offset();
			}
			else
			{
				pos = Scene::SceneManager::get_active_scene()->get_camera()->get_relative_position(this);	
			}

			m_current_sprite->render(pos, m_hitbox);
			Debug::draw_rect(pos, m_hitbox);
			Debug::draw_dot(pos);
		}
	}

	inline void MissileProjectile::focus_this()
	{
		Scene::SceneManager::get_active_scene()->add_game_object(
			Abstract::LayerType::Character, this);
		Scene::SceneManager::get_active_scene()->get_camera()->set_object(this);
		set_enabled();
	}

	inline void MissileProjectile::unfocus_this()
	{
		Scene::SceneManager::get_active_scene()->remove_game_object(
			Abstract::LayerType::Character, this);
		Scene::SceneManager::get_active_scene()->get_camera()->restore_object();
		set_disabled();
	}

	inline void MissileProjectile::fire(
		const Math::Vector2& position,
		const Math::Vector2 velocity,
		const float charged)
	{
		m_position = {position.get_x() + m_hitbox.get_x() + 50.0f, position.get_y() - 50.0f};
		m_fired_position = {position.get_x(), position.get_y()};
		m_speed = 3.0f * charged;

		if(velocity * Math::Vector2{1, 0} == Math::left)
		{
			m_current_sprite = m_left;
			m_hitbox = m_left->get_hitbox();
		}
		else if (velocity * Math::Vector2{1, 0} == Math::right)
		{
			m_current_sprite = m_right;
			m_hitbox = m_right->get_hitbox();
		}

		m_velocity = Math::identity * Math::Vector2{velocity.get_x(), -1};
		focus_this();
	}

	inline void MissileProjectile::on_collision(rigidBody* other)
	{
		projectile::on_collision(other);
		unfocus_this();
	}
}
#endif // PRECISONPROJECTILE_HPP
