#ifndef APPLICATION_H
#define APPLICATION_H
#pragma once

#include "common.h"

namespace Fortress
{
	using namespace Math;
	using namespace Object;

	class Application
	{
	public:
		Application() :
		m_hwnd(nullptr),
		m_hdc(nullptr),
		m_buffer_bitmap(nullptr),
		m_buffer_hdc(nullptr),
		m_playerPos(0.0f, 0.0f) {}

		~Application()
		{
			DeleteObject(m_buffer_bitmap);
			ReleaseDC(m_hwnd, m_buffer_hdc);
			DeleteDC(m_buffer_hdc);
		}
		Application& operator=(const Application&) = delete;

		void initialize(HWND, HDC);
		void update() const;
		void render() const;
		__forceinline static int get_window_width()
		{
			return m_window_size.right - m_window_size.left;
		}

		__forceinline static int get_window_height()
		{
			return m_window_size.bottom - m_window_size.top;
		}

		static void checkWindowFrame(Character& target);

	private:
		inline static RECT m_window_size = {0, 0, 800, 600};
		HWND m_hwnd;
		HDC m_hdc;

		HBITMAP m_buffer_bitmap;
		HDC m_buffer_hdc;

		Vector2 m_playerPos;
	};
}

#endif
