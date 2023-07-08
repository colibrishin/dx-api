#pragma once
#ifndef GROUND_H
#define GROUND_H

#include "object.hpp"
#include "sceneManager.hpp"

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

			if(!_known_grounds.empty())
			{
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
		}

		void render() override;

		bool m_bActive;
	private:
		friend ObjectInternal::_rigidBody;
		// @todo: ground_check is in the _ridgidbody.
		inline static std::vector<ground*> _known_grounds = {};
	};

	inline void ground::render()
	{
		if(m_bActive)
		{
			Rectangle(
				WinAPIHandles::get_buffer_dc(),
				get_x(), 
				get_y(), 
				get_x() + m_hitbox.get_x(),
				get_y() + m_hitbox.get_y());
		}
	}
}

#endif