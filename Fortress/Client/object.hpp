#ifndef OBJECT_HPP
#define OBJECT_HPP
#pragma once

#include "math.h"

namespace ObjectInternal
{
	class _baseObject
	{
	public:
		_baseObject() = delete;
		virtual ~_baseObject() = default;
		_baseObject& operator=(const _baseObject& other) = default;

		virtual void move_left()
		{
			m_position = m_position.left();
		}
		virtual void move_right()
		{
			m_position = m_position.right();
		}
		virtual void move_down()
		{
			m_position = m_position.top();
		}
		virtual void move_up()
		{
			m_position = m_position.bottom();
		}
		float get_x() const
		{
			return m_position.get_x();
		}
		float get_y() const
		{
			return m_position.get_y();
		}
		Math::Vector2 get_position() const
		{
			return m_position;
		}
		virtual Math::Vector2 operator+(const Math::Vector2& vector) const
		{
			return {m_position + vector};
		}
		virtual _baseObject& operator+=(const Math::Vector2& vector)
		{
			m_position += vector;
			return *this;
		}

	protected:
		Math::Vector2 m_position;
		_baseObject(const Math::Vector2& position)
		: m_position(position) {}
	};
}
#endif // OBJECT_HPP
