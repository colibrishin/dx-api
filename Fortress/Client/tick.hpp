#ifndef TICK_H
#define TICK_H
#pragma once

#include <mutex>

namespace Utility
{
	struct Tick
	{
	public:
		Tick() : m_ticked(false) {}

		__forceinline bool is_ticked();
		__forceinline void set_ticked();
		__forceinline void unset_ticked();

	private:
		std::mutex _mutex;
		bool m_ticked;
	};

	bool Tick::is_ticked()
	{
		_mutex.lock();
		const bool ret = m_ticked;
		_mutex.unlock();
		return ret;
	}

	void Tick::set_ticked()
	{
		_mutex.lock();
		m_ticked = true;
		_mutex.unlock();
	}

	void Tick::unset_ticked()
	{
		_mutex.lock();
		m_ticked = false;
		_mutex.unlock();
	}
}

#endif
