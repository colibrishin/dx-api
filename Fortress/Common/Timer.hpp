#ifndef TIMER_HPP
#define TIMER_HPP
#include "entity.hpp"
#include "TimerManager.hpp"

namespace Fortress
{
	/**
	 * \brief A timer that triggered and operates temporarily in specific duration in WinAPI thread.
	 */
	class Timer : public Abstract::entity
	{
	public:
		Timer& operator=(const Timer& other) = default;
		Timer& operator=(Timer&& other) = default;
		Timer(const Timer& other) = default;
		Timer(Timer&& other) = default;
		virtual ~Timer() override = default;

		void initialize();
		void update();
		void toggle();
		void set_duration(const float);
		bool is_started() const;
		virtual void on_timer();
		virtual void stop();

	private:
		friend class TimerManager;

		UINT_PTR m_timer_id;
		float m_duration;
		float m_elapsed;
		bool m_bStarted;

	protected:
		Timer(const std::wstring& name, const float duration, const WPARAM timer_id);
	};
}
#endif // TIMER_HPP
