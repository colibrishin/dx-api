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
			m_destroyed_table(static_cast<int>(m_hitbox.get_y()), std::vector<bool>(static_cast<int>(m_hitbox.get_x()))),
			m_ground_hdc(nullptr),
			m_ground_bitmap(nullptr)
		{
			Ground::initialize();
		}
		Ground& operator=(const Ground& other) = default;
		Ground& operator=(Ground&& other) = default;

		virtual void on_collision(const CollisionCode& collison, const std::shared_ptr<rigidBody>& other) override;

		~Ground() override
		{
			if(m_ground_hdc)
			{
				DeleteObject(m_ground_bitmap);
				ReleaseDC(WinAPIHandles::get_hwnd(), m_ground_hdc);
				DeleteDC(m_ground_hdc);
			}
			rigidBody::~rigidBody();
		}

		void initialize() override;
		void render() override;
		int is_destroyed(const int x, const int y) const;

	private:
		void set_destroyed(const int x, const int y);
		void set_line_destroyed(const int mid_x, const int mid_y, const int n);
		void draw_destroyed(const Math::Vector2& top_left, const float radius) const;

		std::vector<std::vector<bool>> m_destroyed_table;
		HDC m_ground_hdc;
		HBITMAP m_ground_bitmap;
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
					projectile->up_hit_count();
				}
				else
				{
					Debug::Log(L"Projectile hits the destroyed ground");
				}

				// align to top left position, set_line_destroyed will set destroyed in mid point way.
				// @todo: using position top left and mid mix debt is getting higher.
				draw_destroyed(
					{local_position.get_x() - projectile->get_radius() / 2, local_position.get_y()}, 
					projectile->get_radius());
			}
		}

		rigidBody::on_collision(collison, other);
	}

	inline void Ground::initialize()
	{
		m_ground_hdc = CreateCompatibleDC(WinAPIHandles::get_main_dc());
		m_ground_bitmap = CreateCompatibleBitmap(WinAPIHandles::get_main_dc(), m_hitbox.get_x(), m_hitbox.get_y());

		SelectObject(m_ground_hdc, m_ground_bitmap);
		// @todo: replace with image or so.
		Rectangle(m_ground_hdc, 0, 0, m_hitbox.get_x(), m_hitbox.get_y());
		
		rigidBody::initialize();
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

				// Transparent color is the destroyed ground.
				GdiTransparentBlt(
					WinAPIHandles::get_buffer_dc(),
					pos.get_x(),
					pos.get_y(),
					m_hitbox.get_x(),
					m_hitbox.get_y(),
					m_ground_hdc,
					0,
					0,
					m_hitbox.get_x(),
					m_hitbox.get_y(),
					RGB(255,0,255));

				Debug::draw_dot(pos);
			}
		}
	}

	inline int Ground::is_destroyed(const int x, const int y) const
	{
		if(x >= 0 && x < m_hitbox.get_x() && y >= 0 && y < m_hitbox.get_y())
		{
			return m_destroyed_table[y][x];
		}

		return -1;
	}

	inline void Ground::set_destroyed(const int x, const int y)
	{
		for(int i = 0; i < y; ++i)
		{
			if(!m_destroyed_table[i][x])
			{
				m_destroyed_table[i][x] = true;
			}
		}

		m_destroyed_table[y][x] = true;
	}
	inline void Ground::set_line_destroyed(const int mid_x, const int mid_y, const int n)
	{
		int left_x = mid_x - (n / 2);
		int to_right_x = mid_x;

		for(int i = 0; i < n / 2; ++i)
		{
			if(left_x >= 0 && mid_y < static_cast<int>(m_hitbox.get_y()))
			{
				set_destroyed(left_x, mid_y);
				left_x++;
			}
		}
		for(int i = n / 2; i < n; ++i)
		{
			if(to_right_x < m_hitbox.get_x() && mid_y < static_cast<int>(m_hitbox.get_y()))
			{
				set_destroyed(to_right_x, mid_y);
				to_right_x++;
			}
		}
	}

	inline void Ground::draw_destroyed(const Math::Vector2& top_left, const float radius) const
	{
		Graphics m_ground_gdi(m_ground_hdc);
		const SolidBrush removal_brush(Color(255,0,255));

		const RectF ellipse_rect = {
			top_left.get_x(), top_left.get_y(), radius, radius};


		// due to the set_destroyed logic, we need to also remove upper side.
		const RectF upper_ellipse_rect = {
			top_left.get_x(), top_left.get_y() - (radius / 2), radius, radius};

		m_ground_gdi.FillEllipse(&removal_brush, ellipse_rect);
		m_ground_gdi.FillEllipse(&removal_brush, upper_ellipse_rect);
	}
}

#endif
