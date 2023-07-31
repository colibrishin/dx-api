#pragma once
#ifndef GROUND_H
#define GROUND_H

#include <vector>
#include "rigidBody.hpp"
#include "projectile.hpp"
#include "sceneManager.hpp"

namespace Fortress
{
	class Radar;
}

namespace Fortress::Object
{
	enum class GroundState
	{
		NotDestroyed = static_cast<uint8_t>(0),
		Outline,
		Destroyed,
		OutOfBound,
	};

	class Ground : public Abstract::rigidBody
	{
	public:
		Ground(const std::wstring& name, const Math::Vector2& position, const Math::Vector2& size) :
			rigidBody(
				name, 
				position, 
				size, 
				{},
				10000.0f,
				{}, 
				{}, 
				false),
			m_destroyed_table(static_cast<int>(m_hitbox.get_y()), std::vector<GroundState>(static_cast<int>(m_hitbox.get_x()), GroundState::NotDestroyed)),
			m_ground_hdc(nullptr),
			m_ground_bitmap(nullptr)
		{
			Ground::initialize();
		}
		Ground& operator=(const Ground& other) = default;
		Ground& operator=(Ground&& other) = default;

		void on_collision(const CollisionCode& collision, const Math::Vector2& hit_vector, const std::weak_ptr<rigidBody>& other) override;

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

		GroundState safe_is_destroyed(const Math::Vector2& local_position) const;
		void safe_set_destroyed_global(const Math::Vector2& hit_position, const float radius);
		bool safe_is_object_stuck_global(const Math::Vector2& position) const;
		bool safe_is_object_stuck_local(const Math::Vector2& position) const;
		Math::Vector2 safe_nearest_surface(const Math::Vector2& position) const;
		Math::Vector2 safe_orthogonal_surface(const Math::Vector2& global_position, const Math::Vector2& forward) const;
	protected:
		HDC get_ground_hdc() const;
		void unsafe_set_destroyed(const int x, const int y);
		void unsafe_set_destroyed_visual(int x, int y);
		void safe_set_circle_destroyed(const Math::Vector2& center_position, const int radius);
		bool safe_is_projectile_hit(const Math::Vector2& hit_position, const std::weak_ptr<ObjectBase::projectile>& projectile_ptr) const;
		void _debug_draw_destroyed_table() const;

		friend Radar;
		std::vector<std::vector<GroundState>> m_destroyed_table;
		HDC m_ground_hdc;
		HBITMAP m_ground_bitmap;
	};

	inline void Ground::on_collision(
		const CollisionCode& collision, 
		const Math::Vector2& hit_vector, 
		const std::weak_ptr<Abstract::rigidBody>& other)
	{
		if (auto const projectile = 
			std::dynamic_pointer_cast<ObjectBase::projectile>(other.lock()))
		{
			const eHitVector e_vec = Math::translate_hit_vector(hit_vector);
			const auto hit_point = projectile->get_hit_point(e_vec);

			if (projectile->get_max_hit_count() > projectile->get_hit_count() &&
				safe_is_projectile_hit(hit_point, projectile))
			{
				safe_set_destroyed_global(hit_point, projectile->get_radius());

				if (const auto scene = Scene::SceneManager::get_active_scene().lock()) 
				{
					const auto near_objects = scene->is_in_range<ObjectBase::character>(
						hit_point, projectile->get_radius());

					for (const auto character : near_objects) 
					{
						const auto diff = character.lock()->get_center() - hit_point;
						character.lock()->hit(diff.magnitude() / projectile->get_radius());
					}
				}

				projectile->up_hit_count();
				projectile->play_hit_sound();
			}
		}

		rigidBody::on_collision(collision, hit_vector, other);
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

	inline GroundState Ground::safe_is_destroyed(const Math::Vector2& local_position) const
	{
		if(static_cast<int>(local_position.get_x()) >= 0 && 
			static_cast<int>(local_position.get_x()) < m_hitbox.get_x() && 
			static_cast<int>(local_position.get_y()) >= 0 && 
			static_cast<int>(local_position.get_y()) < m_hitbox.get_y())
		{
			return m_destroyed_table
				[static_cast<int>(local_position.get_y())]
				[static_cast<int>(local_position.get_x())];
		}

		return GroundState::OutOfBound;
	}

	inline void Ground::unsafe_set_destroyed(const int x, const int y)
	{
		m_destroyed_table[y][x] = GroundState::Destroyed;
	}

	inline void Ground::unsafe_set_destroyed_visual(const int x, const int y)
	{
		Graphics m_ground_gdi(m_ground_hdc);
		const SolidBrush removal_brush(Color(255,0,255));
		Rect pixel{0, 0, 1, 1};

		pixel.X = x;
		pixel.Y = y;
		m_ground_gdi.FillRectangle(&removal_brush, pixel);
		m_destroyed_table[y][x] = GroundState::Destroyed;
	}

	inline void Ground::safe_set_circle_destroyed(const Math::Vector2& center_position, const int radius)
	{
		const Math::Vector2 start_pos = {center_position.get_x() - radius, center_position.get_y()};
		Math::Vector2 curr_pos = start_pos;
		const Math::Vector2 end_pos = {center_position.get_x() + radius, center_position.get_y()};

		float radian = 0.0f;
		const float sin_max = Math::to_radian(180.0f);
		const float sin_iter = sin_max / (radius * 2.0f);

		while(curr_pos != end_pos)
		{
			const int column_n = static_cast<int>(std::sin(radian) * static_cast<float>(radius * 2));

			const int upper_size = column_n / 2 - 1;
			const int lower_size = column_n / 2;

			for(int i = 0; i < upper_size; ++i)
			{
				if(static_cast<int>(curr_pos.get_x()) >= 0 && 
					static_cast<int>(curr_pos.get_x()) < m_hitbox.get_x() && 
					static_cast<int>(curr_pos.get_y() + i) >= 0 && 
					static_cast<int>(curr_pos.get_y() + i) < m_hitbox.get_y())
				{
					unsafe_set_destroyed(curr_pos.get_x(), curr_pos.get_y() + i);
					unsafe_set_destroyed_visual(curr_pos.get_x(), curr_pos.get_y() + i);
				}
			}

			for(int i = 0; i < lower_size; ++i)
			{
				if(static_cast<int>(curr_pos.get_x()) >= 0 && 
					static_cast<int>(curr_pos.get_x()) < m_hitbox.get_x() && 
					static_cast<int>(curr_pos.get_y() - i) >= 0 && 
					static_cast<int>(curr_pos.get_y() - i) < m_hitbox.get_y())
				{
					unsafe_set_destroyed(curr_pos.get_x(), curr_pos.get_y() - i);
					unsafe_set_destroyed_visual(curr_pos.get_x(), curr_pos.get_y() - i);
				}
			}

			radian += sin_iter;
			curr_pos = {curr_pos.get_x() + 1, curr_pos.get_y()};
		}
	}

	inline bool Ground::safe_is_projectile_hit(
		const Math::Vector2& hit_position,
		const std::weak_ptr<ObjectBase::projectile>& projectile_ptr) const
	{
		if(const auto projectile = projectile_ptr.lock())
		{
			const auto local_position = to_top_left_local_position(hit_position);
			const GroundState ground_status = safe_is_destroyed(local_position);

			if(ground_status == GroundState::NotDestroyed)
			{
				Debug::Log(L"Projectile hits the Ground");
				return true;
			}
			if(ground_status == GroundState::Destroyed)
			{
				Debug::Log(L"Projectile hits the destroyed ground");
				return false;
			}
		}

		return false;
	}

	inline bool Ground::safe_is_object_stuck_global(const Math::Vector2& global_position) const
	{
		const auto local_position = to_top_left_local_position(global_position);
		return safe_is_object_stuck_local(local_position);
	}

	inline bool Ground::safe_is_object_stuck_local(const Math::Vector2& local_position) const
	{
		// @todo: proper oob definition
		Math::Vector2 offsets[4] =
		{
			{-1.0f, 0.0f},
			{1.0f, 0.0f}
		};

		int count = 0;

		for (const auto& offset : offsets) 
		{
			const auto results = safe_is_destroyed(local_position + offset);
			count += results == GroundState::NotDestroyed;
		}

		return count == 4;
	}

	inline Math::Vector2 Ground::safe_nearest_surface(const Math::Vector2& position) const
	{
		const auto o_local_position = to_top_left_local_position(position);
		auto local_position = to_top_left_local_position(position);

		if(local_position.get_y() <= 0)
		{
			return {0, -local_position.get_y()};
		}

		while(local_position.get_y() > 0)
		{
			if(safe_is_destroyed(local_position) == GroundState::Destroyed)
			{
				auto delta = o_local_position - local_position;
				return delta;
			}

			local_position = {local_position.get_x(), local_position.get_y() - 1};
		}

		return {0, -local_position.get_y()};
	}

	inline Math::Vector2 Ground::safe_orthogonal_surface(
		const Math::Vector2& global_position,
		const Math::Vector2& forward) const
	{
		const auto local_position = to_top_left_local_position(global_position);

		for(int x = 0; x < 10; ++x)
		{
			for(int y = 0; y < m_hitbox.get_y(); ++y)
			{
				const float new_x = local_position.get_x() + (forward == Math::left ? x : -x);

				const Math::Vector2 new_pos = 
					{new_x, static_cast<float>(y)};

				if(safe_is_destroyed(new_pos) == GroundState::NotDestroyed)
				{
					return new_pos - local_position;
				}
			}
		}

		return {};
	}

	inline HDC Ground::get_ground_hdc() const
	{
		return m_ground_hdc;
	}

	inline void Ground::safe_set_destroyed_global(
		const Math::Vector2& hit_position,
		const float radius)
	{
		const auto local_position = to_top_left_local_position(hit_position);
		safe_set_circle_destroyed(local_position, radius);
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
