#ifndef TIMERMANAGER_HPP
#define TIMERMANAGER_HPP
#include "object.hpp"
#include "Timer.hpp"

namespace Fortress
{
	class Timer;
}

namespace Fortress::ObjectBase
{
	constexpr int timer_id = 8000;

	class TimerManager
	{
	public:
		template <typename T, typename... Args>
		static std::weak_ptr<T> create(Args... args);
		static void remove(const std::weak_ptr<Timer>& timer);
		static void update(const WPARAM& wParam);
		static void cleanup();

	private:
		inline static int used_timer_id = timer_id;
		inline static std::map<WPARAM, std::shared_ptr<Timer>> m_timers = {};
	};

	template<typename T, typename... Args>
	inline std::weak_ptr<T> TimerManager::create(Args... args)
	{
		m_timers[used_timer_id] = std::make_shared<T>(args..., used_timer_id);
		return std::dynamic_pointer_cast<T>(m_timers[used_timer_id++]);
	}

	inline void TimerManager::remove(const std::weak_ptr<Timer>& timer)
	{
		m_timers.erase(timer.lock()->m_timer_id);
	}

	inline void TimerManager::update(const WPARAM& wParam)
	{
		const auto designated = m_timers[wParam];
		designated->on_timer();
	}

	inline void TimerManager::cleanup()
	{
		for(auto& object : m_timers)
		{
			object.second->reset();
			object.second.reset();
		}
	}
}
#endif // TIMERMANAGER_HPP
