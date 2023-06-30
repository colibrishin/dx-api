#ifndef APPLICATION_H
#define APPLICATION_H
#pragma once

#include <queue>

#include "common.h"

namespace Fortress
{
	using namespace Math;
	using namespace Utility;

	class Application
	{
	public:
		Application() : m_hwnd(nullptr), m_hdc(nullptr), m_playerPos(0.0f, 0.0f) {}

		~Application() = default;
		Application& operator=(const Application&) = delete;

		void initialize(HWND, HDC);
		void run();
		void update();
		void render();

	private:
		__forceinline void checkKeyUpdate();
		void moveRandomly();

		HWND m_hwnd;
		HDC m_hdc;
		Vector2 m_playerPos;
		Tick m_update_tick;

		Tick m_render_tick;
		std::queue<std::function<void()>> m_render_queue;
	};
}

#endif
