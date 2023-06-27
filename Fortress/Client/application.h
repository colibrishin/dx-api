#include "common.h"

#ifndef APPLICATION_H
#define APPLICATION_H

namespace Fortress 
{
	using namespace Math;
	class Application 
	{
	public:
		Application() : m_hwnd(nullptr), m_hdc(nullptr), m_playerPos(0.0f, 0.0f) {};
		~Application();
		void initialize(const HWND, const HDC);
		void run(const UINT message, const WPARAM wparam);
		void update(bool erase = true) const;
		void render();

	private:
		HWND m_hwnd;
		HDC m_hdc;
		Vector2 m_playerPos;
	};
}

#endif