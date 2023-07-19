#pragma once
#ifndef GROUND_H
#define GROUND_H

#include <vector>
#include "rigidBody.hpp"
#include "projectile.hpp"

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
				10000.0f,
				0.0f, 
				0.0f, 
				false,
				false),
			m_destroyed_table(static_cast<int>(m_hitbox.get_y()), std::vector<bool>(static_cast<int>(m_hitbox.get_x())))
		{
		}
		Ground& operator=(const Ground& other) = default;
		Ground& operator=(Ground&& other) = default;

		virtual void on_collision(const CollisionCode& collison, const std::shared_ptr<rigidBody>& other) override;

		~Ground() override
		{
			rigidBody::~rigidBody();
		}

		void render() override;
		bool is_destroyed(const int x, const int y) const;
		void get_explosion_effect(const Math::Vector2& bottom, const float radius);
		void set_destroyed(const int x, const int y);
		void set_line_destroyed(const int mid_x, const int mid_y, const int n);
	private:
		std::vector<std::vector<bool>> m_destroyed_table;
	};

	inline void Ground::on_collision(const CollisionCode& collison, const std::shared_ptr<rigidBody>& other)
	{
		if (auto const projectile = 
			std::dynamic_pointer_cast<ObjectBase::projectile>(other))
		{
			// @todo: multi angle hit detection
			const auto local_position = projectile->get_bottom() - get_top_left();

			// OOB check
			if(!(local_position.get_x() < 0 || local_position.get_x() >= m_hitbox.get_x() || 
				local_position.get_y() < 0 || local_position.get_y() >= m_hitbox.get_y()))
			{
				if(!is_destroyed(
					std::floorf(local_position.get_x()), 
					std::floorf(local_position.get_y())))
				{
					const int y = local_position.get_y();
					const int x = local_position.get_x();

					constexpr float max = Math::PI / 2;
					const int radius = projectile->get_radius();
					const int end_point = y + radius;
					const float inc = max / radius;
					float i = 0;

					for(int height = y; height < end_point; height++)
					{
						const int next_n = std::floorf(radius * cosf(i));
						set_line_destroyed(x, height, next_n);
						i += inc;
					}

					Debug::Log(L"Projectile hits the Ground");
					Debug::draw_dot(other->get_center());
				}
				else
				{
					Debug::Log(L"Projectile hits the destroyed ground");
				}
			}
		}

		rigidBody::on_collision(collison, other);
	}

	inline void Ground::render()
	{
		if (is_active())
		{
			if(const auto camera_ptr = Scene::SceneManager::get_active_scene().lock()->get_camera().lock())
			{
				const auto pos = camera_ptr->get_relative_position(
				std::dynamic_pointer_cast<object>(shared_from_this()));
				Debug::Log(L"Ground pos" + std::to_wstring(pos.get_x()) + std::to_wstring(pos.get_y()));

				const auto char_pos = camera_ptr->get_offset();
				Debug::Log(L"char diff" + std::to_wstring((pos - char_pos).get_x() ) + std::to_wstring((pos - char_pos).get_y()));

				Rectangle(
					WinAPIHandles::get_buffer_dc(),
					pos.get_x(),
					pos.get_y(),
					pos.get_x() + m_hitbox.get_x(),
					pos.get_y() + m_hitbox.get_y());

				// @todo: performance degrading, this should be replaced with intersection removal.
				for(int i = 0; i < m_hitbox.get_y(); ++i)
				{
					for(int j = 0; j < m_hitbox.get_x(); ++j)
					{
						if(m_destroyed_table[i][j])
						{
							Debug::draw_dot({pos.get_x() + j, pos.get_y() + i});
						}
					}
				}

				Debug::draw_dot(pos);
			}
		}
	}

	inline bool Ground::is_destroyed(const int x, const int y) const
	{
		return m_destroyed_table[y][x];
	}

	inline void Ground::set_destroyed(const int x, const int y)
	{
		if(y != 0 && !m_destroyed_table[y - 1][x])
		{
			m_destroyed_table[y - 1][x] = true;
		}
		m_destroyed_table[y][x] = true;
	}
	inline void Ground::set_line_destroyed(const int mid_x, const int mid_y, const int n)
	{
		int left_x = mid_x - (n / 2);
		int to_right_x = mid_x;

		for(int i = 0; i < n / 2; ++i)
		{
			if(left_x >= 0)
			{
				set_destroyed(left_x++, mid_y);
			}
		}
		for(int i = n / 2; i < n; ++i)
		{
			if(to_right_x < m_hitbox.get_x())
			{
				set_destroyed(to_right_x++, mid_y);
			}
		}
	}
}

#endif
