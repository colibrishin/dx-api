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
		virtual ~_baseObject() = default;
		_baseObject& operator=(const _baseObject& other) = default;
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
	protected:
		_baseObject(const std::wstring& name, const Math::Vector2 position, const Math::Vector2 hitbox)
		: _baseEntity(name), m_position(position), m_hitbox(hitbox) {}
		__forceinline static CollisionCode is_collision(const _baseObject* left, const _baseObject* right) noexcept
		{
			// See also: https://www.acmicpc.net/problem/1002

			// force to pivoting from top left to mid point.
			const auto left_mid = Math::Vector2{left->get_x() + left->m_hitbox.get_x() / 2, left->get_y() + left->m_hitbox.get_y() / 2};
			const auto right_mid = Math::Vector2{right->get_x() + right->m_hitbox.get_x() / 2, right->get_y() + right->m_hitbox.get_y() / 2};
			const auto diff = left_mid - right_mid;

			// identical
			if (left_mid == right_mid)
			{
				return CollisionCode::Identical;
			}

			const auto hitbox_diff = left->m_hitbox - right->m_hitbox;
			const auto& hitbox_sum = left->m_hitbox + right->m_hitbox;

			// Too far
			if (hitbox_diff.get_x() > diff.get_x() || hitbox_sum.get_x() < diff.get_x() ||
				hitbox_diff.get_y() > diff.get_y() || hitbox_sum.get_y() < diff.get_y())
			{
				return CollisionCode::None;
			}

			float x = 0;
			float y = 0;

			// X Collision, meet each other in radius or meet each other their inside.
			if (hitbox_diff.get_x() - diff.get_x() < Math::epsilon || hitbox_sum.get_x() - diff.get_x() < Math::epsilon ||
				hitbox_diff.get_x() < diff.get_x() && diff.get_x() < hitbox_sum.get_x())
			{
				x = diff.normalized().get_x();
			}

			if (hitbox_diff.get_y() - diff.get_y() < Math::epsilon || hitbox_sum.get_y() - diff.get_y() < Math::epsilon ||
				hitbox_diff.get_y() < diff.get_y() && diff.get_y() < hitbox_sum.get_y())
			{
				y = diff.normalized().get_y();
			}

			if (std::fabs(x) < Math::epsilon && std::fabs(y) < Math::epsilon)
			{
				Fortress::Debug::Log(L"None");
				return CollisionCode::None;
			}

			// using epsilon instead of zero.
			if (x > 0 && std::fabs(y) < Math::epsilon)
			{
				Fortress::Debug::Log(L"Left");
				return CollisionCode::Left;
			}
			if (x < 0 && std::fabs(y) < Math::epsilon)
			{
				Fortress::Debug::Log(L"Right");
				return CollisionCode::Right;
			}
			if (std::fabs(x) < Math::epsilon && y < 0)
			{
				Fortress::Debug::Log(L"Bottom");
				return CollisionCode::Bottom;
			}
			if (std::fabs(x) < Math::epsilon && y > 0)
			{
				Fortress::Debug::Log(L"Top");
				return CollisionCode::Top;
			}
			if (x > 0 && y < 0)
			{
				Fortress::Debug::Log(L"BottomLeft");
				return CollisionCode::BottomLeft;
			}
			if (x < 0 && y > 0)
			{
				Fortress::Debug::Log(L"TopRight");
				return CollisionCode::TopRight;
			}
			if (x < 0 && y < 0)
			{
				Fortress::Debug::Log(L"BottomRight");
				return CollisionCode::BottomRight;
			}
			if (x > 0 && y > 0)
			{
				Fortress::Debug::Log(L"TopLeft");
				return CollisionCode::TopLeft;
			}

			return CollisionCode::None;
		}
	};
}
#endif // OBJECT_HPP
