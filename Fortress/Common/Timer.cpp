#include "pch.h"
#include "Timer.hpp"

#include "deltatime.hpp"

namespace Fortress
{
	void Timer::initialize()
	{
	}

	void Timer::update()
	{
		if(!m_bStarted)
		{
			return;
		}

		m_elapsed += DeltaTime::get_deltaTime();

		if(m_duration > m_elapsed)
		{
			return;
		}

		on_timer();
	}

	void Timer::toggle()
	{
		m_elapsed = 0.0f;
		m_bStarted = true;
	}

	void Timer::set_duration(const float duration)
	{
		stop();
		m_duration = duration;
	}

	bool Timer::is_started() const
	{
		return m_bStarted;
	}

	void Timer::on_timer()
	{
		stop();
	}

	void Timer::stop()
	{
		m_elapsed = 0.0f;
		m_bStarted = false;
	}

	Timer::Timer(const std::wstring& name, const float duration, const WPARAM timer_id):
		entity(name), m_timer_id(timer_id), m_duration(duration), m_elapsed(0.f), m_bStarted(false)
	{
	}
}
