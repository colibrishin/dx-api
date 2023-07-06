#ifndef APPLICATION_H
#define APPLICATION_H
#pragma once

#include "common.h"
#include "renderQueue.hpp"

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
		void update();
		void render();
	private:
		RenderQueue m_render_queue;

		inline static RECT m_window_size = {0, 0, 800, 600};
		HWND m_hwnd;
		HDC m_hdc;

		HBITMAP m_buffer_bitmap;
		HDC m_buffer_hdc;

		Vector2 m_playerPos;
	};
}

#endif
