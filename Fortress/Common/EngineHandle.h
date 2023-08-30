#ifndef ENGINEHANDLE_H
#define ENGINEHANDLE_H
#include "pch.h"
#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>

using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")

#include "framework.h"
#include <memory>
#include "NetworkMessenger.hpp"

namespace Fortress
{
	class EngineHandle
	{
	public:
		EngineHandle() = default;
		virtual ~EngineHandle()
		{
			if(m_graphics_)
			{
				m_graphics_.reset();
			}
			if(m_font)
			{
				m_font.reset();
			}
			if (m_font_collection)
			{
				m_font_collection.reset();
			}
			GdiplusShutdown(token);
		}

		virtual void initialize(HWND hwnd, HDC hdc);
		virtual int get_window_width() = 0;
		virtual int get_window_height() = 0;
		virtual HDC get_buffer_dc() = 0;
		virtual HDC get_main_dc() = 0;
		virtual std::weak_ptr<Graphics> get_buffer_gdi_handle() = 0;
		virtual HWND get_hwnd() = 0;
		virtual int get_actual_max_y() = 0;
		virtual RECT& get_window_size() = 0;
		virtual std::weak_ptr<Font> get_font() = 0;

		static void set_handle(std::shared_ptr<EngineHandle> h);
		static std::weak_ptr<EngineHandle> get_handle();
		static Network::NetworkMessenger* get_messenger();

	protected:
		GdiplusStartupInput input;
		ULONG_PTR token{};

		RECT m_window_size = {0, 0, 800, 600};
		RECT m_native_size = {0, 0, 0, 0};
		HWND m_hwnd = nullptr;
		HDC m_hdc = nullptr;
		HDC m_buffer_hdc = nullptr;
		HBITMAP m_buffer_bitmap = nullptr;

		std::shared_ptr<Graphics> m_graphics_;
		std::shared_ptr<Font> m_font;
		std::unique_ptr<PrivateFontCollection> m_font_collection;

	private:
		// @todo: probably this is not good way to share the data.
		inline static std::shared_ptr<EngineHandle> winapi_handle = {};
		inline static std::shared_ptr<Network::NetworkMessenger> network_messenger_ = {};
	};

	inline void EngineHandle::initialize(HWND hwnd, HDC hdc)
	{
		GdiplusStartup(&token, &input, NULL);
		network_messenger_ = std::make_shared<Network::NetworkMessenger>();
	}

	inline void EngineHandle::set_handle(std::shared_ptr<EngineHandle> h)
	{
		winapi_handle = std::move(h);
	}

	inline std::weak_ptr<EngineHandle> EngineHandle::get_handle()
	{
		return winapi_handle;
	}

	inline Network::NetworkMessenger* EngineHandle::get_messenger()
	{
		return network_messenger_.get();
	}
}

#endif
