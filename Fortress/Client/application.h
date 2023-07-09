#ifndef APPLICATION_H
#define APPLICATION_H
#pragma once

#include "common.h"
#include "render_queue.hpp"

namespace Fortress
{
	class Application
	{
	public:
		Application() :
			m_hwnd(nullptr),
			m_hdc(nullptr),
			m_buffer_hdc(nullptr)
		{
		}

		~Application() = default;
		Application& operator=(const Application&) = delete;

		void initialize(HWND, HDC);
		void update();
		void render();

	private:
		RenderQueue m_render_queue;
		HWND m_hwnd;
		HDC m_hdc;
		HDC m_buffer_hdc;
	};
}

#endif
