#ifndef PROJECTILETIMER_HPP
#define PROJECTILETIMER_HPP
#include <functional>

#include "common.h"
#include "Timer.hpp"
#include "vector2.hpp"

namespace Fortress
{
	class ProjectileTimer : public Timer
	{
	public:
		ProjectileTimer(
			WPARAM timer_id,
			ProjectileInitFunction func,
			ObjectBase::character* char_this) :
		Timer(L"Next Projectile", 0.1f, timer_id),
		m_count(0),
		m_curr_count(0),
		m_func(std::move(func)),
		m_char_this(char_this)
		{
			initialize();
		}

		inline void on_timer() override;
		void set_count(int count);
		void set_angle(const UnitVector& angle);
		void set_charged(const float);
		void reset();

	private:
		int m_count;
		int m_curr_count;
		ProjectileInitFunction m_func;
		ObjectBase::character* m_char_this;
		UnitVector m_angle;
		float m_charged;
	};

	inline void ProjectileTimer::on_timer()
	{
		Timer::on_timer();

		if(m_curr_count < m_count && m_char_this)
		{
			m_func(m_char_this, m_angle, m_charged);
			m_curr_count++;
			toggle();
		}
	}

	inline void ProjectileTimer::set_count(int count)
	{
		m_count = count;
	}

	inline void ProjectileTimer::set_angle(const UnitVector& angle)
	{
		m_angle = angle;
	}

	inline void ProjectileTimer::set_charged(const float charged)
	{
		m_charged = charged;
	}

	inline void ProjectileTimer::reset()
	{
		m_curr_count = 0;
	}
}
#endif // PROJECTILETIMER_HPP
