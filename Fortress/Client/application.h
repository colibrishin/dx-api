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
		m_window_size{0, 0, 800, 600},
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
		void run();
		void update();
		void render();

	private:
		__forceinline void checkKeyUpdate();
		void checkWindowFrame(Character& target);
		__forceinline int get_window_width() const
		{
			return m_window_size.right - m_window_size.left;
		}
		__forceinline int get_window_height() const
		{
			return m_window_size.bottom - m_window_size.top;
		}

		RECT m_window_size;
		HWND m_hwnd;
		HDC m_hdc;

		HBITMAP m_buffer_bitmap;
		HDC m_buffer_hdc;

		Vector2 m_playerPos;
		std::vector<Character> m_objects;
	};
}

#endif
