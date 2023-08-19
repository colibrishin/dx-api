#pragma once
#ifndef CAMERA_HPP
#define CAMERA_HPP
#include "EngineHandle.h"
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
		Math::Vector2 get_relative_position(const std::weak_ptr<Abstract::object>& obj) const;
		Math::Vector2 get_offset() const;
		Math::Vector2 get_offset(const Math::Vector2& hitbox) const;
		std::weak_ptr<Abstract::object> get_locked_object() const;

	private:
		Math::Vector2 m_target_center = {};
		Math::Vector2 m_window_size = {};
		Math::Vector2 m_center_position = {};
		std::weak_ptr<Abstract::object> m_lock_target;
	};

	inline void Camera::update()
	{
		m_center_position = m_window_size / 2;

		if(const auto target_ptr = m_lock_target.lock())
		{
			m_target_center = target_ptr->get_center();
		}
	}

	inline void Camera::initialize()
	{
		m_window_size = Math::Vector2{
			static_cast<float>(EngineHandle::get_handle().lock()->get_window_width()),
			static_cast<float>(EngineHandle::get_handle().lock()->get_actual_max_y())};

		m_center_position = m_window_size / 2;
		m_lock_target.reset();
	}

	inline void Camera::set_object(const std::weak_ptr<Abstract::object>& obj)
	{
		m_lock_target = obj;
	}

	inline Math::Vector2 Camera::get_relative_position(const std::weak_ptr<Abstract::object>& obj) const
	{
		const auto diff = m_target_center - obj.lock()->get_center();
		return (m_center_position - diff) - (obj.lock()->m_hitbox / 2);
	}

	inline Math::Vector2 Camera::get_offset() const
	{
		return m_center_position;
	}

	inline Math::Vector2 Camera::get_offset(const Math::Vector2& hitbox) const
	{
		return m_center_position - hitbox / 2;
	}

	inline std::weak_ptr<Abstract::object> Camera::get_locked_object() const
	{
		return m_lock_target;
	}
}
#endif // CAMERA_HPP
