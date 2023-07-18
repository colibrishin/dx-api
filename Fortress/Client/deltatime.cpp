#include "deltatime.hpp"
#include "winapihandles.hpp"

namespace Fortress
{
	/**
	 * \brief Shows the fps counter on the screen.
	 */
	void DeltaTime::render()
	{
		wchar_t szFloat[50]{};
		float fps = 1.0f / m_deltaTime;

		swprintf(szFloat, 50, L"DeltaTime: %5f", fps);
		int strlen = wcsnlen_s(szFloat, 50);

		TextOut(WinAPIHandles::get_buffer_dc(), 10, 10, szFloat, strlen);
	}
}
