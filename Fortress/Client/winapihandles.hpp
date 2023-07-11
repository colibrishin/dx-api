#ifndef WINAPIHANDLES_H
#define WINAPIHANDLES_H
#pragma once
#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")

#include "framework.h"

namespace Fortress
{
	class WinAPIHandles
	{
	public:
		WinAPIHandles()
		{
		    GdiplusStartup(&token, &input, NULL);
		}
		~WinAPIHandles()
		{
			DeleteObject(m_buffer_bitmap);
			ReleaseDC(m_hwnd, m_buffer_hdc);
			DeleteDC(m_buffer_hdc);
			GdiplusShutdown(token);
		}

		static void initialize(HWND hwnd, HDC hdc);
		static int get_window_width();
		static int get_window_height();
		static HDC get_buffer_dc();
		static HDC get_main_dc();
		static HWND get_hwnd();
		static int get_actual_max_y();
		static RECT& get_window_size();

	private:
		GdiplusStartupInput input;
		ULONG_PTR token{};
		inline static RECT m_window_size = {0, 0, 800, 600};
		inline static RECT m_native_size = {0, 0, 0, 0};
		inline static HWND m_hwnd = nullptr;
		inline static HDC m_hdc = nullptr;
		inline static HDC m_buffer_hdc = nullptr;
		inline static HBITMAP m_buffer_bitmap = nullptr;
	};
}

namespace Fortress
{
	__forceinline void WinAPIHandles::initialize(const HWND hwnd, const HDC hdc)
	{
		m_hwnd = hwnd;
		m_hdc = hdc;

		AdjustWindowRect(&m_window_size, WS_OVERLAPPEDWINDOW, false);
		SetWindowPos(m_hwnd, nullptr, 0, 0, get_window_width(),
		             get_window_height(), 0);
		ShowWindow(m_hwnd, true);
		UpdateWindow(m_hwnd);

		GetClientRect(m_hwnd, &m_native_size);

		m_buffer_bitmap = CreateCompatibleBitmap(m_hdc, get_window_width(), get_window_height());
		m_buffer_hdc = CreateCompatibleDC(m_hdc);

		// m_buffer_hdc selects the buffer bitmap.
		const auto defaultBitmap = SelectObject(m_buffer_hdc, m_buffer_bitmap);
		// free the temporary handle.
		DeleteObject(defaultBitmap);
	}

	__forceinline int WinAPIHandles::get_window_width()
	{
		return m_window_size.right - m_window_size.left;
	}

	__forceinline int WinAPIHandles::get_window_height()
	{
		return m_window_size.bottom - m_window_size.top;
	}

	__forceinline HDC WinAPIHandles::get_buffer_dc()
	{
		return m_buffer_hdc;
	}

	__forceinline HDC WinAPIHandles::get_main_dc()
	{
		return m_hdc;
	}

	__forceinline HWND WinAPIHandles::get_hwnd()
	{
		return m_hwnd;
	}

	__forceinline int WinAPIHandles::get_actual_max_y()
	{
		return m_native_size.bottom - m_native_size.top;
	}

	__forceinline RECT& WinAPIHandles::get_window_size()
	{
		return m_window_size;
	}
}

#endif
