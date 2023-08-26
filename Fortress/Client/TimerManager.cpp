#include "TimerManager.hpp"
#include "Timer.hpp"

namespace Fortress
{
	void TimerManager::remove(const std::weak_ptr<Timer>& timer)
	{
		m_timers.erase(timer.lock()->m_timer_id);
	}

	void TimerManager::update()
	{
		for(const auto& timer : m_timers)
		{
			timer.second->update();
		}
	}

	void TimerManager::cleanup()
	{
		for(auto& object : m_timers)
		{
			object.second->stop();
			object.second.reset();
		}
	}
}
