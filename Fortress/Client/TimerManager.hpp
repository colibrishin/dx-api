#ifndef TIMERMANAGER_HPP
#define TIMERMANAGER_HPP
#include "object.hpp"

namespace Fortress
{
	class Timer;

	constexpr int timer_id = 8000;

	class TimerManager
	{
	public:
		template <typename T, typename... Args>
		static std::weak_ptr<T> create(Args... args);
		static void remove(const std::weak_ptr<Timer>& timer);
		static void update();
		static void cleanup();

	private:
		inline static int used_timer_id = timer_id;
		inline static std::map<WPARAM, std::shared_ptr<Timer>> m_timers = {};
	};

	template<typename T, typename... Args>
	inline std::weak_ptr<T> TimerManager::create(Args... args)
	{
		const auto created = std::make_shared<T>(used_timer_id, args...);
		m_timers[used_timer_id] = created;
		used_timer_id++;
		return created;
	}
}
#endif // TIMERMANAGER_HPP
