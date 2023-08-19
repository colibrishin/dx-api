#pragma once
#ifndef DELTATIME_H
#define DELTATIME_H

#include <windows.h>
#include "common.h"
#include "../Common/EngineHandle.h"

namespace Fortress
{
	class DeltaTime
	{
	public:
		DeltaTime() = delete;
		DeltaTime& operator=(const DeltaTime& other) = delete;
		DeltaTime& operator=(DeltaTime&& other) = delete;
		~DeltaTime() = delete;
		DeltaTime(const DeltaTime& other) = delete;
		DeltaTime(const DeltaTime&& other) = delete;

		__forceinline static void initialize();
		__forceinline static void update();
		__forceinline static void render();
		__forceinline static float get_deltaTime();

	private:
		inline static LARGE_INTEGER m_cpu_frequency = {};
		inline static LARGE_INTEGER m_prev_frequency = {};
		inline static LARGE_INTEGER m_curr_frequency = {};
		inline static float m_deltaTime = {};
	};
}

namespace Fortress
{
	/**
	 * \brief Updates the DeltaTime variables, update should be done in the first time of every cycles.
	 */
	__forceinline void DeltaTime::update()
	{
		QueryPerformanceCounter(&m_curr_frequency);

		const float delta = static_cast<float>(m_curr_frequency.QuadPart - m_prev_frequency.QuadPart);
		m_deltaTime = delta / static_cast<float>(m_cpu_frequency.QuadPart);

		m_prev_frequency = m_curr_frequency;
	}

	inline void DeltaTime::render()
	{
		wchar_t szFloat[50]{};
		float fps = 1.0f / m_deltaTime;

		swprintf(szFloat, 50, L"DeltaTime: %5f", fps);
		int strlen = wcsnlen_s(szFloat, 50);

		TextOut(EngineHandle::get_handle().lock()->get_buffer_dc(), 10, 10, szFloat, strlen);
	}

	/**
	 * \brief Get DeltaTime of current system.
	 * \return A deltaTime value, which is the duration for the one frame.
	 */
	__forceinline float DeltaTime::get_deltaTime()
	{
		return m_deltaTime;
	}

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
}

#endif //DELTATIME_H
