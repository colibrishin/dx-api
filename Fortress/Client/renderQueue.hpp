#pragma once
#ifndef RENDERQUEUE_HPP
#define RENDERQUEUE_HPP
#include <functional>
#include <queue>

namespace Fortress
{
	class RenderQueue
	{
	public:
		void push(const unsigned int priority, const std::function<void()>& func)
		{
			m_render_queue.emplace(priority, func);
		}

		bool empty() const
		{
			return m_render_queue.empty();
		}

		std::function<void()> get_next()
		{
			if (m_render_queue.empty())
			{
				// @todo: redundant return of function.
				return [](){};
			}

			auto func = m_render_queue.top();
			m_render_queue.pop();
			return func.second;
		}
	private:
		struct PriorityCompare
		{
			bool operator()(
				const std::pair<int, std::function<void()>>& left, 
				const std::pair<int, std::function<void()>>& right) const
			{
				return left.first < right.first;
			}
		};
		std::priority_queue<
			std::pair<int, std::function<void()>>,
			std::vector<std::pair<int, std::function<void()>>>,
			PriorityCompare> m_render_queue;
	};
}

#endif