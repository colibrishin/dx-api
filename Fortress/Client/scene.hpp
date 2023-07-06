#ifndef SCENE_H
#define SCENE_H
#pragma once

#include "baseEntity.hpp"
#include "framework.h"

namespace Fortress 
{
	class _scene : public _baseEntity
	{
	public:
		_scene(const std::wstring& name) : _baseEntity(name)
		{
		}

		virtual void initialize();
		virtual void update();
		virtual void render();
	private:
	};
}

#endif