#ifndef OBJECT_HPP
#define OBJECT_HPP
#pragma once

#include "vector2.hpp"
#include "debug.hpp"
#include "entity.hpp"

namespace Fortress::Abstract
{
	class object : public entity
	{
	public:
		Math::Vector2 m_hitbox;
		Math::Vector2 m_position;

		object() = delete;
		object& operator=(const object& other) = delete;
		object& operator=(object&& other) = delete;

		__forceinline virtual void render();
		__forceinline virtual void move_left();
		__forceinline virtual void move_right();
		__forceinline virtual void move_down();
		__forceinline virtual void move_up();
		__forceinline float get_x() const;
		__forceinline float get_y() const;
		__forceinline Math::Vector2 get_position() const;
		__forceinline virtual Math::Vector2 operator+(const Math::Vector2& vector) const;
		__forceinline virtual object& operator+=(const Math::Vector2& vector);

	protected:
		object(const std::wstring& name, const Math::Vector2 position, const Math::Vector2 hitbox)
			: entity(name), m_hitbox(hitbox), m_position(position)
		{
			object::initialize();
		}

		inline static std::vector<object*> _known_objects = {};

		__forceinline virtual void initialize();
		__forceinline ~object() override;
		__forceinline object(const object& other);

		template <typename T>
		__forceinline static std::vector<T*> is_in_range(
			const Math::Vector2& top_left, const Math::Vector2& hit_box, float radius);
	};
}

namespace Fortress::Abstract
{
	void object::initialize()
	{
		_known_objects.push_back(this);
	}

	void object::render()
	{
	}

	void object::move_left()
	{
		m_position = m_position.left();
	}

	void object::move_right()
	{
		m_position = m_position.right();
	}

	void object::move_down()
	{
		m_position = m_position.top();
	}

	void object::move_up()
	{
		m_position = m_position.bottom();
	}

	float object::get_x() const
	{
		return m_position.get_x();
	}

	float object::get_y() const
	{
		return m_position.get_y();
	}

	Math::Vector2 object::get_position() const
	{
		return m_position;
	}

	Math::Vector2 object::operator+(const Math::Vector2& vector) const
	{
		return {m_position + vector};
	}

	object& object::operator+=(const Math::Vector2& vector)
	{
		m_position += vector;
		return *this;
	}

	object::~object()
	{
		if (!_known_objects.empty())
		{
			_known_objects.erase(
				std::remove_if(
					_known_objects.begin(),
					_known_objects.end(),
					[this](const object* r)
					{
						return r == this;
					}),
				_known_objects.end());
		}

		entity::~entity();
	}

	inline object::object(const object& other) :
		entity(other),
		m_hitbox(other.m_hitbox),
		m_position(other.m_position)
	{
	}

	template <typename T>
	std::vector<T*> object::is_in_range(
		const Math::Vector2& top_left,
		const Math::Vector2& hit_box,
		const float radius)
	{
		std::vector<T*> ret = {};

		static_assert(std::is_base_of_v<object, T>);

		const auto mid_point = Math::Vector2{
			top_left.get_x() + hit_box.get_x() / 2,
			top_left.get_y() + hit_box.get_y() / 2
		};

		for (const auto obj : _known_objects)
		{
			if (typeid(obj) != typeid(T))
			{
				continue;
			}

			if (obj->m_position.get_x() <= mid_point.get_x() + radius &&
				obj->m_position.get_x() >= mid_point.get_x() - radius &&
				obj->m_position.get_y() <= mid_point.get_y() + radius &&
				obj->m_position.get_y() >= mid_point.get_y() - radius)
			{
				ret.push_back(obj);
			}
		}

		return ret;
	}
}
#endif // OBJECT_HPP
