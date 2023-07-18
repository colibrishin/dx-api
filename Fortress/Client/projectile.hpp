#pragma once
#ifndef PROJECTILE_HPP
#define PROJECTILE_HPP

#include "character.hpp"
#include "rigidBody.hpp"

namespace Fortress::ObjectBase
{
	constexpr float default_explosion_radius = 10.0f;

	class projectile : public Abstract::rigidBody
	{
	public:
		projectile() = delete;
		projectile& operator=(const projectile& other) = default;
		projectile& operator=(projectile&& other) = default;
		projectile(const projectile& other) = default;
		projectile(projectile&& other) = default;
		~projectile() override = default;

		void initialize() override;
		virtual void fire(const Math::Vector2& position, const Math::Vector2& bottom, const Math::Vector2& velocity, const float charged);
		__forceinline virtual void update() override;
		virtual void on_collision(const std::shared_ptr<rigidBody>& other) override;
		virtual void focus_this();
		virtual void unfocus_this();
		virtual void render() override;
		int get_radius() const;

		float get_damage() const;

	protected:
		projectile(
			const std::wstring& name,
			const std::wstring& short_name,
			const Math::Vector2& position,
			const Math::Vector2& velocity,
			const float mass,
			const float speed,
			const float acceleration,
			const float damage,
			const float radius) :
			rigidBody(name, position, {}, velocity, mass, speed, acceleration, true, true),
			m_damage(damage),
			m_radius(radius),
			m_texture(short_name)
		{
			projectile::initialize();
		}

	private:
		float m_damage;
		int m_radius;
		Texture<GifWrapper> m_texture;
		std::weak_ptr<GifWrapper> m_current_sprite;
		Math::Vector2 m_fired_position;
	};

	__forceinline void projectile::update()
	{
		if(m_position.get_y() >= WinAPIHandles::get_actual_max_y() - 50.0f)
		{
			unfocus_this();
		}

		rigidBody::update();
	}
}
#endif // PROJECTILE_HPP
