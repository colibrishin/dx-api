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
		virtual void fire(const Math::Vector2& position, const Math::Vector2& velocity, const float charged);
		__forceinline virtual void update() override;
		virtual void on_collision(const CollisionCode& collision, const Math::Vector2& hit_vector, const std::weak_ptr<rigidBody>& other);
		virtual void focus_this();
		virtual void unfocus_this();
		virtual void render() override;
		virtual void prerender();
		const character* get_origin() const;

		void up_hit_count();
		int get_hit_count() const;
		int get_max_hit_count() const;
		virtual void play_fire_sound() = 0;
		virtual void play_hit_sound() = 0;

		const std::weak_ptr<GifWrapper>& get_current_sprite() const;
		int get_radius() const;
		float get_damage() const;
		const Math::Vector2& get_fired_position() const;
	protected:
		projectile(
			const character* shooter,
			const std::wstring& name,
			const std::wstring& short_name,
			const Math::Vector2& position,
			const Math::Vector2& velocity,
			const float mass,
			const float speed,
			const float acceleration,
			const float damage,
			const float radius,
			const int hit_count) :
			rigidBody(name, position, {30.0f, 30.0f}, velocity, mass, speed, acceleration, true),
			m_damage(damage),
			m_radius(radius),
			m_max_hit_count(hit_count),
			m_curr_hit_count(0),
			m_shooter(shooter),
			m_texture(short_name),
			m_sound_pack(short_name)
		{
			projectile::initialize();
		}

	private:
		void explosion_near_ground(const Math::Vector2& hit_vector) const;

		float m_damage;
		int m_radius;
		const int m_max_hit_count;
		int m_curr_hit_count;
		const character* m_shooter;
		Texture<GifWrapper> m_texture;
		std::weak_ptr<GifWrapper> m_current_sprite;
		Math::Vector2 m_fired_position;

	protected:
		SoundPack m_sound_pack;
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
