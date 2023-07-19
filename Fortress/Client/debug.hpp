#pragma once
#ifndef DEBUG_HPP
#define DEBUG_HPP
#include "common.h"
#include "winapihandles.hpp"

namespace Fortress
{
	class Debug final
	{
	public:
		static void initialize(HDC hdc)
		{
			m_hdc = hdc;
		}

		static void Log(const std::wstring& str)
		{
			m_render_queue.push([str]()
			{
				TextOut(m_hdc, x, y, str.c_str(), str.length());
				y += y_movement;
				y %= WinAPIHandles::get_actual_max_y();
			});
		}

		static void draw_line(const Math::Vector2 left, const Math::Vector2 right);

		static void draw_dot(const Math::Vector2 point);
		static void draw_circle(Math::Vector2 point, float radius);
		static void draw_rect(Math::Vector2 point, Math::Vector2 size);

		static void render()
		{
			while (!m_render_queue.empty())
			{
				m_render_queue.front()();
				m_render_queue.pop();
			}

			y = y_initial;
		}

	private:
		static constexpr int y_movement = 15;
		static constexpr int y_initial = 30;
		inline static int x = 100;
		inline static int y = y_initial;
		inline static HDC m_hdc;
		inline static std::queue<std::function<void()>> m_render_queue;
	};

	inline void Debug::draw_line(const Math::Vector2 left, const Math::Vector2 right)
	{
		m_render_queue.push([left, right]()
		{
			MoveToEx(m_hdc, left.get_x(), left.get_y(), nullptr);
			LineTo(m_hdc, right.get_x(), right.get_y());
		});
	}

	inline void Debug::draw_dot(const Math::Vector2 point)
	{
		m_render_queue.push([point]()
		{
			Ellipse(m_hdc, point.get_x(), point.get_y(), point.get_x() + 5, point.get_y() + 5);
		});
	}

	inline void Debug::draw_circle(const Math::Vector2 point, const float radius)
	{
		m_render_queue.push([point, radius]()
		{
			Ellipse(m_hdc, point.get_x(), point.get_y(), point.get_x() + radius, point.get_y() + radius);
		});
	}

	inline void Debug::draw_rect(const Math::Vector2 point, const Math::Vector2 size)
	{
		m_render_queue.push([point, size]()
		{
			// transparent rectangle is pain.
			draw_line(point, point + Math::Vector2{size.get_x(), 0});
			draw_line(point + Math::Vector2{size.get_x(), 0}, point + size);
			draw_line(point + size, point + Math::Vector2{0, size.get_y()});
			draw_line(point + Math::Vector2{0, size.get_y()}, point);
		});
	}
}

#endif
