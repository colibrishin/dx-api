#ifndef TIMER_HPP
#define TIMER_HPP
#include <map>
#include <functional>
#include "framework.h"
#include "entity.hpp"
#include "winapihandles.hpp"

namespace Fortress
{
	namespace ObjectBase
	{
		class TimerManager;
	}

	

	/**
	 * \brief A timer that triggered and operates temporarily in specific duration in WinAPI thread.
	 */
	class Timer : public Abstract::entity
	{
	public:
		Timer& operator=(const Timer& other) = default;
		Timer& operator=(Timer&& other) = default;
		Timer(const Timer& other) = default;
		Timer(Timer&& other) = default;
		virtual ~Timer() override;

		void initialize();
		void start(const std::function<void()>& on_end);
		void set_duration(const UINT&);
		bool is_started() const;
		virtual void on_timer();
		virtual void reset();

	private:
		friend ObjectBase::TimerManager;
		UINT_PTR m_timer_id;
		UINT m_duration;
		bool m_bStarted;

	protected:
		Timer(const std::wstring& name, const UINT duration, const WPARAM timer_id) :
			entity(name), m_timer_id(timer_id), m_duration(duration), m_bStarted(false)
		{
			initialize();
		}

		std::function<void()> m_reserved_function{};
	};

	inline Timer::~Timer()
	{
		entity::~entity();
	}

	inline void Timer::initialize()
	{
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

	inline void Timer::set_duration(const UINT& duration)
	{
		m_duration = duration;
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
