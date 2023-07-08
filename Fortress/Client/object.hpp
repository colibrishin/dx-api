#ifndef OBJECT_HPP
#define OBJECT_HPP
#pragma once

#include "math.h"
#include "baseEntity.hpp"
#include "debug.hpp"

namespace ObjectInternal
{
	class _baseObject : public Fortress::_baseEntity
	{
	public:
		Math::Vector2 m_hitbox;
		Math::Vector2 m_position;

		_baseObject() = delete;
		_baseObject& operator=(const _baseObject& other) = default;

		virtual ~_baseObject() override
		{
			if(!_known_objects.empty())
			{
				_known_objects.erase(
				std::remove_if(
					_known_objects.begin(),
					_known_objects.end(),
					[this](const _baseObject* r)
					{
						return r == this;
					}),
				_known_objects.end());
			}
		}

		virtual void initialize()
		{
			_known_objects.push_back(this);
		}

		virtual void render() {};

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
		_baseObject(const std::wstring& name, const Math::Vector2 position, const Math::Vector2 hitbox)
		: _baseEntity(name), m_position(position), m_hitbox(hitbox)
		{
			_baseObject::initialize();
		}

		inline static std::vector<_baseObject*> _known_objects = {};

		__forceinline static std::vector<_baseObject*> is_in_range(
			const Math::Vector2& top_left, 
			const Math::Vector2& hit_box,
			const float radius)
		{
			std::vector<_baseObject*> ret = {};

			const auto mid_point = Math::Vector2{
				top_left.get_x() + hit_box.get_x() / 2,
				top_left.get_y() + hit_box.get_y() / 2};

			for(const auto obj : _known_objects)
			{
				if(obj->m_position.get_x() <= mid_point.get_x() + radius && 
					obj->m_position.get_x() >= mid_point.get_x() - radius &&
					obj->m_position.get_y() <= mid_point.get_y() + radius && 
					obj->m_position.get_y() >= mid_point.get_y() - radius)
				{
					ret.push_back(obj);
				}
			}

			return ret;
		}
	};
}
#endif // OBJECT_HPP
