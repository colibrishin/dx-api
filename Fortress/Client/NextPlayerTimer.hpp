#ifndef NEXTPLAYERTIMER_HPP
#define NEXTPLAYERTIMER_HPP
#include "Timer.hpp"

namespace Fortress
{
	class NextPlayerTimer : public Timer
	{
	public:
		NextPlayerTimer() : Timer(L"Next Player", 2000) { }
		inline void on_timer() override;
	};

	inline void NextPlayerTimer::on_timer()
	{
		Timer::on_timer();
	}
}
#endif // NEXTPLAYERTIMER_HPP
