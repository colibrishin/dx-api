#ifndef APPLICATION_H
#define APPLICATION_H
#pragma once

#include "framework.h"

#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>

#pragma comment (lib,"Gdiplus.lib")

namespace Fortress
{
	using namespace Gdiplus;

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
		static void cleanup();

	private:
		HWND m_hwnd;
		HDC m_hdc;
		HDC m_buffer_hdc;
	};
}

#endif
