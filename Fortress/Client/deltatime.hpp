#pragma once
#ifndef DELTATIME_H
#define DELTATIME_H

#include <exception>
#include <algorithm>
#include "framework.h"
#include "math.h"

namespace Fortress
{
	class DeltaTime
	{
	public:
		static void initialize();
		__forceinline static void update();
		static void render();
		__forceinline static float get_deltaTime();

	private:
		static LARGE_INTEGER m_cpu_frequency;
		static LARGE_INTEGER m_prev_frequency;
		static LARGE_INTEGER m_curr_frequency;
		static float m_deltaTime;
	};

	void DeltaTime::update()
	{
		QueryPerformanceCounter(&m_curr_frequency);

		const float delta = static_cast<float>(m_curr_frequency.QuadPart - m_prev_frequency.QuadPart);
		m_deltaTime = delta / static_cast<float>(m_cpu_frequency.QuadPart);

		m_prev_frequency = m_curr_frequency;
	}

	float DeltaTime::get_deltaTime()
	{
		return m_deltaTime;
	}
}

#endif //DELTATIME_H
