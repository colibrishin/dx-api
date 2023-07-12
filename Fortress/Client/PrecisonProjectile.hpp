#pragma once
#ifndef PRECISONPROJECTILE_HPP
#define PRECISONPROJECTILE_HPP

#include "GifWrapper.hpp"
#include "projectile.hpp"
#include "math.h"

namespace Fortress::Object
{
	class PrecisionProjectile final : public ObjectBase::projectile
	{
	public:
		PrecisionProjectile() : projectile(L"Precision Projectile", {}, {10.0f, 10.0f}, Math::identity, 2.0f, 0.0f, 10.0f)
		{
			PrecisionProjectile::initialize();
		}
		~PrecisionProjectile() override
		{
			projectile::~projectile();
		}

		void initialize() override;
		void update() override;
		void render() override;
		void fire(const Math::Vector2& position, const Math::Vector2& velocity, const float charged);
		virtual void on_collision(rigidBody* other) override;

	private:
		GifWrapper* m_image{};
	};

	inline void PrecisionProjectile::initialize()
	{
		projectile::initialize();

		m_image = nullptr;
	}

	inline void PrecisionProjectile::update()
	{
		projectile::update();
	}

	inline void PrecisionProjectile::render()
	{
		if(is_active())
		{
			projectile::render();

			auto pos = Scene::SceneManager::get_active_scene()->get_camera()->get_relative_position(this);

			Rectangle(
				WinAPIHandles::get_buffer_dc(),
				pos.get_x(),
				pos.get_y(),
				pos.get_x() + m_hitbox.get_x(),
				pos.get_y() + m_hitbox.get_y());
		}
	}

	inline void PrecisionProjectile::fire(
		const Math::Vector2& position,
		const Math::Vector2& velocity,
		const float charged)
	{
		m_position = {position.get_x() + m_hitbox.get_x() + 50.0f, position.get_y() - 50.0f};
		m_speed = 3.0f * charged;
		m_velocity = Math::identity * Math::Vector2{velocity.get_x(), -1};
		set_enabled();
		Scene::SceneManager::get_active_scene()->add_game_object(
			Abstract::LayerType::Character, this);
	}

	inline void PrecisionProjectile::on_collision(rigidBody* other)
	{
		projectile::on_collision(other);

		set_disabled();
		Scene::SceneManager::get_active_scene()->remove_game_object(
			Abstract::LayerType::Character, this);
	}
}
#endif // PRECISONPROJECTILE_HPP
