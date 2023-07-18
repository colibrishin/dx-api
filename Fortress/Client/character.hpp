#pragma once
#ifndef CHARACTER_HPP
#define CHARACTER_HPP

#include "GifWrapper.hpp"
#include "rigidBody.hpp"
#include "common.h"
#include "sceneManager.hpp"
#include "Texture.hpp"

namespace Fortress::ObjectBase
{
	class projectile;

	constexpr float character_full_hp = 100.0f;
	constexpr float character_full_mp = 100.0f;

	class character : public Abstract::rigidBody
	{
	public:
		character() = delete;
		character& operator=(const character& other) = default;
		character& operator=(character&& other) = default;
		character(const character& other) = default;
		character(character&& other) = default;
		~character() override = default;

		virtual void initialize() override;
		void hit(const std::weak_ptr<projectile>& p);
		void update() override;
		void render() override;
		virtual void firing();
		virtual void shoot();
		float get_charged_power() const;
		virtual void move() override;

		virtual void on_collision(const CollisionCode& collision, const std::shared_ptr<rigidBody>& other) override;
		virtual void on_nocollison() override;

		void set_current_sprite(const std::wstring& name, const std::wstring& orientation);
		void set_sprite_offset(const std::wstring& name, const std::wstring& orientation, const Math::Vector2& offset);
		const std::wstring& get_current_sprite_name() const;

		Math::Vector2 get_offset() const;

		virtual void move_left() override;
		virtual void move_right() override;
		virtual void stop() override;

		float get_hp_percentage() const;
		float get_mp_percentage() const;

	private:
		float m_hp;
		float m_mp;
		float m_power;
		bool m_bGrounded;

		Math::Vector2 m_offset;

		Texture<GifWrapper> m_texture;
		std::weak_ptr<GifWrapper> m_current_sprite;

	protected:
		character(
			const std::wstring& name,
			const std::wstring& short_name,
			const Math::Vector2 offset,
			const Math::Vector2 position,
			const Math::Vector2 velocity,
			const float mass,
			const float speed,
			const float acceleration,
			const int hp,
			const int mp)
			: rigidBody(name, position, {}, velocity, mass, speed, acceleration, true, true),
			  m_hp(hp),
			  m_mp(mp),
		      m_power(1.0f),
		      m_bGrounded(false),
			  m_offset(offset),
			  m_texture(short_name)
		{
			character::initialize();
		}
	};
}

#endif
