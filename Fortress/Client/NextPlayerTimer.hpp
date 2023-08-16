#ifndef NEXTPLAYERTIMER_HPP
#define NEXTPLAYERTIMER_HPP
#include "Timer.hpp"

namespace Fortress
{
	class NextPlayerTimer : public Timer
	{
	public:
		NextPlayerTimer(
			WPARAM timer_id, 
			NextPlayerTimerFunction func,
			Round* timer_this) :
			Timer(L"Next Player", 2.0f, timer_id),
			m_on_timer(std::move(func)),
			m_this(timer_this)
		{
			initialize();
		}
		inline void on_timer() override;
	private:
		NextPlayerTimerFunction m_on_timer;
		Round* m_this;
	};

	inline void NextPlayerTimer::on_timer()
	{
		Timer::on_timer();
		m_on_timer(m_this);
	}
}
#endif // NEXTPLAYERTIMER_HPP
