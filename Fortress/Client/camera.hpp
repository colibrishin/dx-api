#pragma once
#ifndef CAMERA_HPP
#define CAMERA_HPP
#include "object.hpp"
#include "vector2.hpp"

namespace Fortress
{
	class Camera
	{
	public:
		void update();
		void initialize();
		void set_object(Abstract::object* obj);
		Math::Vector2 get_relative_position(const Abstract::object* obj) const;
		Math::Vector2 get_offset() const;
		Abstract::object* get_locked_object() const;

	private:
		Math::Vector2 m_window_size = {};
		Math::Vector2 m_center_position = {};
		Abstract::object* m_lock_target = nullptr;
	};

	inline void Camera::update()
	{
		if(m_lock_target)
		{
			m_center_position = 
				(m_window_size / 2) - Math::Vector2{m_lock_target->m_hitbox.get_x() / 2, m_lock_target->m_hitbox.get_y() / 2};
		}
		else
		{
			m_center_position = m_window_size / 2;
		}
	}

	inline void Camera::initialize()
	{
		m_window_size = Math::Vector2{
			static_cast<float>(WinAPIHandles::get_window_width()),
			static_cast<float>(WinAPIHandles::get_actual_max_y())};

		m_center_position = m_window_size / 2;
		m_lock_target = nullptr;
	}

	inline void Camera::set_object(Abstract::object* obj)
	{
		m_lock_target = obj;
	}

	inline Math::Vector2 Camera::get_relative_position(const Abstract::object* obj) const
	{
		const auto diff = m_lock_target->get_position() - obj->get_position();
		return m_center_position - diff;
	}

	inline Math::Vector2 Camera::get_offset() const
	{
		return m_center_position;
	}

	inline Abstract::object* Camera::get_locked_object() const
	{
		return m_lock_target;
	}
}
#endif // CAMERA_HPP
