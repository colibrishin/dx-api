#ifndef APPLICATION_H
#define APPLICATION_H
#pragma once

#include "resourceManager.hpp"

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
		static void cleanup();

		static std::weak_ptr<Font> get_font();

	private:
		HWND m_hwnd;
		HDC m_hdc;
		HDC m_buffer_hdc;

		inline static std::shared_ptr<Font> m_font;
		std::unique_ptr<PrivateFontCollection> m_font_collection;
	};
}

#endif
