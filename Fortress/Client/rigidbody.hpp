#pragma once
#ifndef RIGIDBODY_HPP
#define RIGIDBODY_HPP

#include <string>
#include "object.hpp"
#include "Texture.hpp"

namespace Fortress::Abstract
{
	class rigidBody : public object
	{
	public:
		Math::Vector2 m_velocity;
		float m_acceleration;

		// rigidBody should be used with inheritance
		rigidBody() = delete;
		rigidBody& operator=(const rigidBody& other) = default;
		rigidBody& operator=(rigidBody&& other) = default;
		rigidBody(const rigidBody& other) = default;
		rigidBody(rigidBody&& other) = default;
		~rigidBody() override = default;

		void initialize() override;
		void update() override;
		static void block_window_frame(const std::weak_ptr<rigidBody>& target);

		virtual void on_collision(const CollisionCode& collison, const std::shared_ptr<rigidBody>& other);
		virtual void on_nocollison();
		static CollisionCode is_collision(const std::weak_ptr<object>& left, const std::weak_ptr<object>& right) noexcept;

		void set_speed(const float speed);

		void move_down() override;
		void move_left() override;
		void move_right() override;
		void move_up() override;
		virtual void stop();

	private:
		float m_speed;
		float m_curr_speed;
		bool m_bGravity{};
		bool m_bClipping{};

		void apply_gravity();
		bool update_reflection(CollisionCode code) noexcept;

	protected:
		rigidBody(
			const std::wstring& name,
			const Math::Vector2& position, 
			const Math::Vector2& hitbox,
			const Math::Vector2& velocity,
			const float& mass,
			const float& speed, 
			const float& acceleration, 
			const bool& gravity, 
			const bool& clipping);
		virtual void move();

		void reset_current_speed();
		void reset_current_gravity_speed();
		void disable_gravity();
		void enable_gravity();


		float m_gravity_speed;
		float m_gravity_acceleration;
	};
}

#endif // RIGIDBODY_HPP
