#pragma once
#ifndef GROUND_H
#define GROUND_H

#include <vector>
#include "rigidBody.hpp"
#include "projectile.hpp"
#include "sceneManager.hpp"

namespace Fortress::Object
{
	enum class GroundState
	{
		NotDestroyed = static_cast<uint8_t>(0),
		Outline,
		Destroyed,
		OutOfBound,
	};

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
				false),
			m_destroyed_table(static_cast<int>(m_hitbox.get_y()), std::vector<GroundState>(static_cast<int>(m_hitbox.get_x()), GroundState::NotDestroyed)),
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
		GroundState is_destroyed(const int x, const int y) const;

	private:
		void set_destroyed(const int x, const int y);
		void set_destroyed_visual(int x, int y);
		void set_line_destroyed(const int mid_x, const int mid_y, const int n);
		bool is_projectile_hit(const Math::Vector2& local_position, const std::weak_ptr<ObjectBase::projectile>& projectile_ptr);
		void _debug_draw_destroyed_table() const;

		std::vector<std::vector<GroundState>> m_destroyed_table;
		HDC m_ground_hdc;
		HBITMAP m_ground_bitmap;
	};

	inline void Ground::on_collision(const CollisionCode& collison, const std::shared_ptr<rigidBody>& other)
	{
		if (auto const projectile = 
			std::dynamic_pointer_cast<ObjectBase::projectile>(other))
		{
			// order matters.
			const Math::Vector2 local_positions[] = 
			{
				projectile->get_bottom() - get_top_left(),
				projectile->get_left() - get_top_left(),
				projectile->get_right() - get_top_left(),
				projectile->get_top() - get_top_left(),
			};

			for(const auto& position : local_positions)
			{
				if(is_projectile_hit(position, projectile) ||
					projectile->get_max_hit_count() <= projectile->get_hit_count())
				{
					projectile->up_hit_count();
					break;
				}
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

	inline GroundState Ground::is_destroyed(const int x, const int y) const
	{
		if(x >= 0 && x < m_hitbox.get_x() && y >= 0 && y < m_hitbox.get_y())
		{
			return m_destroyed_table[y][x];
		}

		return GroundState::OutOfBound;
	}

	inline void Ground::set_destroyed(const int x, const int y)
	{
		m_destroyed_table[y][x] = GroundState::Destroyed;
	}

	inline void Ground::set_destroyed_visual(const int x, const int y)
	{
		static Graphics m_ground_gdi(m_ground_hdc);
		const SolidBrush removal_brush(Color(255,0,255));
		Rect pixel{0, 0, 1, 1};

		pixel.X = x;
		pixel.Y = y;
		m_ground_gdi.FillRectangle(&removal_brush, pixel);
		m_destroyed_table[y][x] = GroundState::Destroyed;
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
				set_destroyed_visual(left_x, mid_y);
				left_x++;
			}
		}
		for(int i = n / 2; i < n; ++i)
		{
			if(to_right_x < m_hitbox.get_x() && mid_y < static_cast<int>(m_hitbox.get_y()))
			{
				set_destroyed(to_right_x, mid_y);
				set_destroyed_visual(to_right_x, mid_y);
				to_right_x++;
			}
		}
	}

	inline bool Ground::is_projectile_hit(
		const Math::Vector2& local_position,
		const std::weak_ptr<ObjectBase::projectile>& projectile_ptr)
	{
		if(const auto projectile = projectile_ptr.lock())
		{
			// OOB check
			if(!(local_position.get_x() < 0 || local_position.get_x() >= m_hitbox.get_x() || 
				local_position.get_y() < 0 || local_position.get_y() >= m_hitbox.get_y()))
			{
				const GroundState ground_status = is_destroyed(
					std::floorf(local_position.get_x()), 
					std::floorf(local_position.get_y()));

				if(ground_status == GroundState::NotDestroyed)
				{
					const int y = local_position.get_y();
					const int x = local_position.get_x();

					constexpr float max = Math::PI;
					const float radius = projectile->get_radius();
					const float end_point = y + (radius * 2);
					const float inc = max / (radius * 2);
					float i = 0;

					for(float height = y - radius; height < end_point; height++)
					{
						if (height >= 0)
						{
							const int next_n = std::floorf(radius * sinf(i));
							set_line_destroyed(x, height, next_n);
						}
						i += inc;
					}

					Debug::Log(L"Projectile hits the Ground");
					return true;
				}
				else if(ground_status == GroundState::Destroyed)
				{
					Debug::Log(L"Projectile hits the destroyed ground");
					return false;
				}
			}
		}

		return false;
	}

	inline void Ground::_debug_draw_destroyed_table() const
	{
		Graphics m_ground_gdi(m_ground_hdc);
		const SolidBrush not_destroyed_brush(Color(0,255,0));
		const SolidBrush destroyed_brush(Color(255,0,0));

		for(int i = 0; i < m_hitbox.get_y(); ++i)
		{
			for(int j = 0; j < m_hitbox.get_x(); ++j)
			{
				Rect pixel = {
					j,
					i,
					1,
					1};

				m_ground_gdi.FillRectangle(
					&(m_destroyed_table[i][j] == GroundState::NotDestroyed ? not_destroyed_brush : destroyed_brush),
					pixel);
			}
		}
	}
}

#endif
