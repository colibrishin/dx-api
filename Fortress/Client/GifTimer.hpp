#ifndef GIFTIMER_HPP
#define GIFTIMER_HPP
#include "Timer.hpp"

namespace Fortress
{
	class GifTimer : public Timer
	{
	public:
		GifTimer(WPARAM timer_id) :
			Timer(L"Gif Timer", 100, timer_id)
		{
		}

		inline void on_timer() override;
	};

	inline void GifTimer::on_timer()
	{
		Timer::on_timer();
	}
}
#endif // GIFTIMER_HPP
