#ifndef GIFTIMER_HPP
#define GIFTIMER_HPP
#include "GifWrapper.h"
#include "common.h"
#include "Timer.hpp"
#include <functional>

namespace Fortress
{
	class GifTimer : public Timer
	{
	public:
		GifTimer(
			WPARAM timer_id, 
			GifOnTimerFunction func,
			GifWrapper* gif_this) :
			Timer(L"Gif Timer", 100, timer_id),
			m_on_timer(std::move(func)),
			m_this(gif_this)
		{
			initialize();
		}

		inline void on_timer() override;
	private:
		GifOnTimerFunction m_on_timer;
		GifWrapper* m_this;
	};

	inline void GifTimer::on_timer()
	{
		m_on_timer(m_this);
		Timer::on_timer();
	}
}
#endif // GIFTIMER_HPP
