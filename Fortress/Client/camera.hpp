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
		void set_object(const std::weak_ptr<Abstract::object>& obj);
		void restore_object();
		Math::Vector2 get_relative_position(const std::weak_ptr<Abstract::object>& obj) const;
		Math::Vector2 get_offset() const;
		std::weak_ptr<Abstract::object> get_locked_object() const;

	private:
		Math::Vector2 m_window_size = {};
		Math::Vector2 m_center_position = {};
		std::weak_ptr<Abstract::object> m_lock_target;
		std::weak_ptr<Abstract::object> m_backup_target;
	};

	inline void Camera::update()
	{
		m_center_position = m_window_size / 2;
	}

	inline void Camera::initialize()
	{
		m_window_size = Math::Vector2{
			static_cast<float>(WinAPIHandles::get_window_width()),
			static_cast<float>(WinAPIHandles::get_actual_max_y())};

		m_center_position = m_window_size / 2;
		m_lock_target.reset();
	}

	inline void Camera::set_object(const std::weak_ptr<Abstract::object>& obj)
	{
		m_backup_target = m_lock_target;
		m_lock_target = obj;
	}

	inline void Camera::restore_object()
	{
		m_lock_target = m_backup_target;
	}

	inline Math::Vector2 Camera::get_relative_position(const std::weak_ptr<Abstract::object>& obj) const
	{
		if(const auto target_ptr = m_lock_target.lock())
		{
			const auto diff = target_ptr->get_center() - obj.lock()->get_center();
			return m_center_position - diff;
		}

		return m_center_position;
	}

	inline Math::Vector2 Camera::get_offset() const
	{
		return m_center_position;
	}

	inline std::weak_ptr<Abstract::object> Camera::get_locked_object() const
	{
		return m_lock_target;
	}
}
#endif // CAMERA_HPP
