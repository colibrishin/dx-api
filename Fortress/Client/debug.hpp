#pragma once
#ifndef DEBUG_HPP
#define DEBUG_HPP
#include "framework.h"
#include <functional>
#include <queue>

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
				y += 15;
				y %= WinAPIHandles::get_actual_max_y();
			});
		}
		static void render()
		{
			while(!m_render_queue.empty())
			{
				m_render_queue.front()();
				m_render_queue.pop();
			}

			y = 15;
		}
	private:
		inline static int x = 100;
		inline static int y = 15;
		inline static HDC m_hdc;
		inline static std::queue<std::function<void()>> m_render_queue;
	};
}

#endif