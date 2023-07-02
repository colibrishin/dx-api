#ifndef APPLICATION_H
#define APPLICATION_H
#pragma once

#include "common.h"

namespace Fortress
{
	constexpr static int OBJECT_COUNT = 100;
	constexpr static float speed = 2000.0f;

	using namespace Math;
	using namespace Utility;
	using namespace Object;

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
		bool updateCharacterCollision(Character& target);
		void reflectiveMove(Character& target);

		HWND m_hwnd;
		HDC m_hdc;
		Vector2 m_playerPos;
		Character m_objects[OBJECT_COUNT];
		Tick m_update_tick;
	};
}

#endif
