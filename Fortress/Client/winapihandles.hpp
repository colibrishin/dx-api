#ifndef WINAPIHANDLES_H
#define WINAPIHANDLES_H
#pragma once
#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>

#pragma comment (lib,"Gdiplus.lib")

#include "framework.h"
#include <memory>
#include "../Common/EngineHandle.h"

namespace Fortress
{
	using namespace Gdiplus;

	class WinAPIHandles : public EngineHandle
	{
	public:
		WinAPIHandles() = default;
		~WinAPIHandles() override
		{
			DeleteObject(m_buffer_bitmap);
			ReleaseDC(m_hwnd, m_buffer_hdc);
			DeleteDC(m_buffer_hdc);
		}

		void initialize(HWND hwnd, HDC hdc) override;
		int get_window_width() override;
		int get_window_height() override;
		HDC get_buffer_dc() override;
		HDC get_main_dc() override;
		std::weak_ptr<Graphics> get_buffer_gdi_handle() override;
		HWND get_hwnd() override;
		int get_actual_max_y() override;
		RECT& get_window_size() override;
		std::weak_ptr<Font> get_font() override;
	};
}

namespace Fortress
{
	__forceinline void WinAPIHandles::initialize(const HWND hwnd, const HDC hdc)
	{
		EngineHandle::initialize(hwnd, hdc);

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

		m_graphics_.reset(Graphics::FromHDC(m_buffer_hdc));

		m_font_collection = std::make_unique<PrivateFontCollection>();

		const std::filesystem::path font_path = "./resources/font/ark-pixel-10px-monospaced-ko.ttf";

		if(m_font_collection->AddFontFile(font_path.native().c_str()) != Ok)
		{
			throw std::exception("Unable to load font file.");
		}

		m_font = std::make_unique<Font>(
					L"Ark Pixel 10px Monospaced ko",
					50,
					FontStyleRegular,
					UnitPixel,
					m_font_collection.get());
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

	__forceinline std::weak_ptr<Graphics> WinAPIHandles::get_buffer_gdi_handle()
	{
		return m_graphics_;
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

	inline std::weak_ptr<Font> WinAPIHandles::get_font()
	{
		return m_font;
	}
}

#endif
