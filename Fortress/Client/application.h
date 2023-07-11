#ifndef APPLICATION_H
#define APPLICATION_H
#pragma once

#include "common.h"
#include "winapihandles.hpp"

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
		WinAPIHandles m_win_handles;
		HWND m_hwnd;
		HDC m_hdc;
		HDC m_buffer_hdc;
	};
}

#endif
