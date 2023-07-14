#pragma once
#ifndef RIGIDBODY_HPP
#define RIGIDBODY_HPP

#include <vector>
#include <string>
#include "object.hpp"

namespace Fortress::Abstract
{
	enum class CollisionCode
	{
		None = 0,
		Identical = 1,
		Top = 2,
		Bottom = 4,
		Left = 8,
		Right = 16,
		TopLeft = 10,
		TopRight = 18,
		BottomLeft = 12,
		BottomRight = 20,
	};

	class rigidBody : public object
	{
	public:
		Math::Vector2 m_velocity;
		float m_speed;
		float m_acceleration;

		// rigidBody should be used with inheritance
		rigidBody() = delete;
		rigidBody& operator=(const rigidBody& other) = default;
		rigidBody& operator=(rigidBody&& other) = default;

		void initialize() override;
		void update() override;
		static void block_window_frame(rigidBody* target);

		virtual void on_collision(rigidBody* other);
		static CollisionCode is_collision(const object* left, const object* right) noexcept;

		void move_down() override;
		void move_left() override;
		void move_right() override;
		void move_up() override;
		virtual void stop();

	private:
		float m_curr_speed;
		bool m_bGravity{};
		bool m_bClipping{};

		void apply_gravity();
		bool update_reflection(CollisionCode code) noexcept;

	protected:
		rigidBody(const std::wstring& name, Math::Vector2 position, Math::Vector2 hitbox, Math::Vector2 velocity,
		          float speed, float acceleration, bool gravity, bool clipping);
		~rigidBody() override;
		rigidBody(const rigidBody& other) = default;
		virtual void move();

		float m_gravity_speed;
		float m_gravity_acceleration;
	};
}

#endif // RIGIDBODY_HPP
