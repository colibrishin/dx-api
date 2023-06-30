#include "deltatime.hpp"

namespace Fortress 
{
	LARGE_INTEGER DeltaTime::m_cpu_frequency = {};
	LARGE_INTEGER DeltaTime::m_prev_frequency = {};
	LARGE_INTEGER DeltaTime::m_curr_frequency = {};
	float DeltaTime::m_deltaTime = 0.0f;

	void DeltaTime::initialize()
	{
		if (!QueryPerformanceFrequency(&m_cpu_frequency))
		{
			throw std::exception("Deltatime error : QueryPerformanceFrequency, Deltatime unsupported cpu");
		}

		m_prev_frequency = m_cpu_frequency;
		m_curr_frequency = m_cpu_frequency;
	}

	void DeltaTime::render(HDC hdc)
	{
		static float interval = 0.0f;

		interval += m_deltaTime;
		if (interval >= 1.0f)
		{
			wchar_t szFloat[50]{};
			float fps = 1.0f / m_deltaTime;

			swprintf(szFloat, 50, L"DeltaTime: %5f fps", fps);
			int strlen = wcsnlen_s(szFloat, 50);

			TextOut(hdc, 10, 10, szFloat, strlen);
			interval = 0.0f;
		}
	}
}