#pragma once
#ifndef GROUND_H
#define GROUND_H

#include "rigidBody.hpp"

namespace Fortress::Object
{
	class Ground final : public Abstract::rigidBody
	{
	public:
		Ground() :
			rigidBody(
				L"Ground", 
				{0.0f, 300.0f}, 
				{1000.0f, 100.0f}, 
				{}, 
				0.0f, 
				0.0f, 
				false,
				false)
		{
		}
		Ground& operator=(const Ground& other) = default;
		Ground& operator=(Ground&& other) = default;

		~Ground() override
		{
			rigidBody::~rigidBody();
		}

		void render() override;
	};

	inline void Ground::render()
	{
		if (is_active())
		{
			const auto pos = Scene::SceneManager::get_active_scene()->get_camera()->get_relative_position(this);

			Rectangle(
				WinAPIHandles::get_buffer_dc(),
				pos.get_x(),
				pos.get_y(),
				pos.get_x() + m_hitbox.get_x(),
				pos.get_y() + m_hitbox.get_y());
		}
	}
}

#endif
