#pragma once
#ifndef GROUND_H
#define GROUND_H

#include "object.hpp"

namespace ObjectInternal
{
	class _rigidBody;
}

namespace Object
{
	class ground final : public ObjectInternal::_baseObject
	{
	public:
		ground() :
		_baseObject(L"Ground", {0.0f, 300.0f}, {1000.0f, 100.0f}), m_bActive(true)
		{
			_known_grounds.push_back(this);
		}

		~ground() override
		{
			_baseObject::~_baseObject();

			_known_grounds.erase(
			std::remove_if(
				_known_grounds.begin(),
				_known_grounds.end(),
				[this](const ground* r)
				{
					return r == this;
				}),
			_known_grounds.end());
		}

		bool m_bActive;
	private:
		friend ObjectInternal::_rigidBody;
		// @todo: ground_check is in the _ridgidbody.
		inline static std::vector<ground*> _known_grounds = {};
	};
}

#endif