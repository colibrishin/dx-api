#ifndef WINAPIHANDLES_H
#define WINAPIHANDLES_H
#pragma once

#include "framework.h"
#include "object.h"

class WinAPIHandles
{
public:
	~WinAPIHandles()
	{
		DeleteObject(m_buffer_bitmap);
		ReleaseDC(m_hwnd, m_buffer_hdc);
		DeleteDC(m_buffer_hdc);
	}

	__forceinline static void initialize(const HWND hwnd, const HDC hdc)
	{
		m_hwnd = hwnd;
		m_hdc = hdc;

		AdjustWindowRect(&m_window_size, WS_OVERLAPPEDWINDOW, false);
		SetWindowPos(m_hwnd, nullptr, 0, 0, get_window_width(),
		             get_window_height(), 0);
		ShowWindow(m_hwnd, true);
		UpdateWindow(m_hwnd);

		m_buffer_bitmap = CreateCompatibleBitmap(m_hdc, get_window_width(), get_window_height());
		m_buffer_hdc = CreateCompatibleDC(m_hdc);

		// m_buffer_hdc selects the buffer bitmap.
		const auto defaultBitmap = SelectObject(m_buffer_hdc, m_buffer_bitmap);
		// free the temporary handle.
		DeleteObject(defaultBitmap);
	}

	__forceinline static int get_window_width()
	{
		return m_window_size.right - m_window_size.left;
	}

	__forceinline static int get_window_height()
	{
		return m_window_size.bottom - m_window_size.top;
	}

	__forceinline static HDC get_buffer_dc()
	{
		return m_buffer_hdc;
	}

	__forceinline static int get_actual_max_y()
	{
		const int topmenu_size = GetSystemMetrics(SM_CXFIXEDFRAME) * 2 + GetSystemMetrics(SM_CYMENU) + GetSystemMetrics(SM_CYCAPTION);
		return get_window_height() - topmenu_size;
	}

	__forceinline static void block_window_frame(Object::Character& target)
	{
		const float x = target.m_hitbox.get_x() * 2;
		const float y = target.m_hitbox.get_y() * 2;

		if(target.get_x() <= 0)
		{
			target.m_velocity = target.m_velocity.reflect_x();
			target.m_position += {1.0f, 0.0f};
		}
		else if(target.get_x() >= get_window_width() - x)
		{
			target.m_velocity = target.m_velocity.reflect_x();
			target.m_position -= {1.0f, 0.0f};
		}
		if(target.get_y() <= 0)
		{
			target.m_velocity = target.m_velocity.reflect_y();
			target.m_position += {0.0f, 1.0f};
		}
		else if (target.get_y() >= get_actual_max_y() - y)
		{
			target.m_velocity = target.m_velocity.reflect_y();
			target.m_position -= {0.0f, 1.0f};
		}
	}

private:
	inline static RECT m_window_size = {0, 0, 800, 600};
	inline static HWND m_hwnd = nullptr;
	inline static HDC m_hdc = nullptr;
	inline static HDC m_buffer_hdc = nullptr;
	inline static HBITMAP m_buffer_bitmap = nullptr;
};

#endif