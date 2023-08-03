#ifndef TIMER_HPP
#define TIMER_HPP
#include <map>

#include "framework.h"
#include "entity.hpp"
#include "winapihandles.hpp"

namespace Fortress
{
	constexpr int timer_id = 18000;

	/**
	 * \brief A timer that triggered and operates temporarily in specific duration in WinAPI thread.
	 */
	class Timer : public Abstract::entity
	{
	public:
		Timer(const std::wstring& name, const UINT duration) :
			entity(name), m_timer_id(0), m_duration(duration), m_bStarted(false)
		{
			initialize();
		}

		Timer& operator=(const Timer& other) = default;
		Timer& operator=(Timer&& other) = default;
		Timer(const Timer& other) = default;
		Timer(Timer&& other) = default;
		virtual ~Timer() override;

		void initialize();
		void start(const std::function<void()>& on_end);
		bool is_started() const;
		virtual void on_timer();
		void reset();

		inline static std::map<UINT_PTR, Timer*> registered_timer = {};

	private:
		UINT_PTR m_timer_id;
		UINT m_duration;
		inline static int used_timer_id = timer_id;
		bool m_bStarted;

		std::function<void()> m_reserved_function{};
	};

	inline Timer::~Timer()
	{
		registered_timer.erase(m_timer_id);
		entity::~entity();
	}

	inline void Timer::initialize()
	{
		m_timer_id = used_timer_id++;
		registered_timer[m_timer_id] = this;
	}

	inline void Timer::start(const std::function<void()>& on_end)
	{
		if(!m_bStarted)
		{
			SetTimer(WinAPIHandles::get_hwnd(), m_timer_id, m_duration, nullptr);
			m_bStarted = true;
			m_reserved_function = on_end;	
		}
	}

	inline bool Timer::is_started() const
	{
		return m_bStarted;
	}

	inline void Timer::on_timer()
	{
		KillTimer(WinAPIHandles::get_hwnd(), m_timer_id);
		if(m_reserved_function != nullptr)
		{
			m_reserved_function();
			m_reserved_function = nullptr;
		}

		m_bStarted = false;
	}

	inline void Timer::reset()
	{
		m_bStarted = false;
		m_reserved_function = nullptr;
		KillTimer(WinAPIHandles::get_hwnd(), m_timer_id);
	}
}
#endif // TIMER_HPP
