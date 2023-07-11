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
		object& operator=(const object& other) = default;
		object& operator=(object&& other) = default;

		__forceinline virtual void render();
		virtual void update();
		__forceinline virtual void move_left();
		__forceinline virtual void move_right();
		__forceinline virtual void move_down();
		__forceinline virtual void move_up();
		__forceinline float get_x() const;
		__forceinline float get_y() const;
		__forceinline Math::Vector2 get_position() const;
		__forceinline virtual Math::Vector2 operator+(const Math::Vector2& vector) const;
		__forceinline virtual object& operator+=(const Math::Vector2& vector);

		bool is_active() const;
		void set_disabled();
		void set_enabled();
	protected:
		object(const std::wstring& name, const Math::Vector2 position, const Math::Vector2 hitbox);

		__forceinline virtual void initialize();
		__forceinline ~object() override;
		__forceinline object(const object& other);

	private:
		bool m_bActive;
	};
}

namespace Fortress::Abstract
{
	void object::initialize()
	{
	}

	void object::render()
	{
	}

	inline void object::update()
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

	inline object::object(const std::wstring& name, const Math::Vector2 position, const Math::Vector2 hitbox): entity(name), m_hitbox(hitbox), m_position(position)
	{
		object::initialize();
	}

	object::~object()
	{
		entity::~entity();
	}

	inline object::object(const object& other) :
		entity(other),
		m_hitbox(other.m_hitbox),
		m_position(other.m_position),
		m_bActive(true)
	{
	}

	inline bool object::is_active() const
	{
		return m_bActive;
	}

	inline void object::set_disabled()
	{
		m_bActive = false;
	}

	inline void object::set_enabled()
	{
		m_bActive = true;
	}
}
#endif // OBJECT_HPP
