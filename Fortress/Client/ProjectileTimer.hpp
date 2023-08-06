#ifndef PROJECTILETIMER_HPP
#define PROJECTILETIMER_HPP
#include <functional>

#include "Timer.hpp"
#include "vector2.hpp"

namespace Fortress
{
	class ProjectileTimer : public Timer
	{
	public:
		// @todo: maybe this might be changed.
		ProjectileTimer() :
			Timer(L"Next Projectile", 100),
			m_count(0),
			m_curr_count(0)
		{
		}

		inline void on_timer() override;
		void set_count(int count);
		void reset() override;
	private:
		int m_count;
		int m_curr_count;
	};

	inline void ProjectileTimer::on_timer()
	{
		if(m_curr_count < m_count)
		{
			Timer::on_timer();
			start(m_reserved_function);
			m_curr_count++;
		}
	}

	inline void ProjectileTimer::set_count(int count)
	{
		m_count = count;
	}

	inline void ProjectileTimer::reset()
	{
		m_curr_count = 0;
		Timer::reset();
	}
}
#endif // PROJECTILETIMER_HPP
