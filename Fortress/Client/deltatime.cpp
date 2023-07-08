#include "deltatime.hpp"

#include "winapihandles.hpp"

namespace Fortress
{
	LARGE_INTEGER DeltaTime::m_cpu_frequency = {};
	LARGE_INTEGER DeltaTime::m_prev_frequency = {};
	LARGE_INTEGER DeltaTime::m_curr_frequency = {};
	float DeltaTime::m_deltaTime = 0.0f;

	/**
	 * \brief Initialize the DeltaTime variables.
	 */
	void DeltaTime::initialize()
	{
		if (!QueryPerformanceFrequency(&m_cpu_frequency))
		{
			throw std::exception("Deltatime error : QueryPerformanceFrequency, Deltatime unsupported cpu");
		}

		QueryPerformanceCounter(&m_curr_frequency);
		QueryPerformanceCounter(&m_prev_frequency);
	}

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
