#ifndef NEXTPLAYERTIMER_HPP
#define NEXTPLAYERTIMER_HPP
#include "Timer.hpp"

namespace Fortress
{
	class NextPlayerTimer : public Timer
	{
	public:
		NextPlayerTimer(WPARAM timer_id) : Timer(L"Next Player", 2000, timer_id) { }
		inline void on_timer() override;
	};

	inline void NextPlayerTimer::on_timer()
	{
		Timer::on_timer();
	}
}
#endif // NEXTPLAYERTIMER_HPP
