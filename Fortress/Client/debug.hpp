#pragma once
#ifndef DEBUG_HPP
#define DEBUG_HPP
#include "framework.h"
#include <functional>
#include <queue>

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
				TextOut(m_hdc, 100, 100, str.c_str(), str.length());
			});
		}
		static void render()
		{
			while(!m_render_queue.empty())
			{
				m_render_queue.front()();
				m_render_queue.pop();
			}
		}
	private:
		inline static HDC m_hdc = nullptr;
		inline static std::queue<std::function<void()>> m_render_queue;
	};
}

#endif