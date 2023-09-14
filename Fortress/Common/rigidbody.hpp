#pragma once
#ifndef RIGIDBODY_HPP
#define RIGIDBODY_HPP

#include <string>
#include "object.hpp"
#include "Texture.hpp"

namespace Fortress::Abstract
{
	constexpr float max_gravity_speed = 1000000000.0f;

	class rigidBody : public object
	{
	public:
		Math::Vector2 m_velocity;
		Math::Vector2 m_acceleration;

		// rigidBody should be used with inheritance
		rigidBody() = delete;
		rigidBody& operator=(const rigidBody& other) = default;
		rigidBody& operator=(rigidBody&& other) = default;
		rigidBody(const rigidBody& other) = default;
		rigidBody(rigidBody&& other) = default;
		~rigidBody() override = default;

		void initialize() override;
		void update() override;
		void render() override;
		virtual void prerender();
		static void block_window_frame(const std::weak_ptr<rigidBody>& target);

		virtual void on_collision(
			const CollisionCode& collision, 
			const GlobalPosition& collision_point, 
			const std::weak_ptr<rigidBody>& other);

		virtual void on_nocollison();
		static CollisionCode is_collision(const std::weak_ptr<object>& left, const std::weak_ptr<object>& right) noexcept;

		void set_speed(const Math::Vector2& speed);
		Math::Vector2 get_speed();
		virtual void set_hitbox(const Math::Vector2& hitbox);

		void move_down() override;
		void move_left() override;
		void move_right() override;
		void move_up() override;
		virtual void stop();

		void set_movement_pitch_radian(const float pitch);
		void set_user_pitch_radian(const float pitch);
		float get_movement_pitch_radian() const;
		float get_user_pitch_radian() const;

		bool is_moving_toward(const rigidBody& other) const;
		bool is_facing_toward(const rigidBody& other) const;

		const Math::Vector2& get_velocity() const;
		const Math::Vector2& get_offset() const;
		const Math::Vector2& get_backward_offset() const;
		Math::Vector2 get_mixed_offset() const;
		Math::Vector2 get_velocity_offset() const;
		Math::Vector2 get_offset_forward_position() const;
		Math::Vector2 get_offset_backward_position() const;
		Math::Vector2 get_offset_bottom_forward_position() const;
		Math::Vector2 get_offset_bottom_backward_position() const;
		Math::Vector2 get_offset_top_forward_position() const;
		Math::Vector2 get_velocity_forward_position() const;

		void set_offset(const Math::Vector2& offset);
	private:
		float m_movement_pitch_radian;
		float m_user_pitch_radian;
		Math::Vector2 m_speed;
		Math::Vector2 m_curr_speed;
		bool m_bGravity{};

		Math::Vector2 m_offset;

		void apply_gravity();

	protected:
		rigidBody(
			const std::wstring& name,
			const Math::Vector2& position, 
			const Math::Vector2& hitbox,
			const Math::Vector2& velocity,
			const float& mass,
			const Math::Vector2& speed, 
			const Math::Vector2& acceleration, 
			const bool& gravity);
		virtual void move();

		void modify_current_speed(const SpeedVector& speed);

		void reset_current_speed();
		void reset_current_gravity_speed();
		void disable_gravity();
		void enable_gravity();


		float m_gravity_speed;
		float m_gravity_acceleration;
	};
}

#endif // RIGIDBODY_HPP
