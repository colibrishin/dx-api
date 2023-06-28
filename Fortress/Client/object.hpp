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
			position = position.left();
		}
		virtual void move_right()
		{
			position = position.right();
		}
		virtual void move_down()
		{
			position = position.top();
		}
		virtual void move_up()
		{
			position = position.bottom();
		}

	protected:
		Math::Vector2 position;
		_baseObject(const float x, const float y)
		: position(x, y) {}
	};
}
#endif // OBJECT_HPP
