#ifndef ANIMATIONQUEUE_HPP
#define ANIMATIONQUEUE_HPP

#include <set>
#include <memory>

#include "deltatime.hpp"
#include "GifWrapper.hpp"

namespace Fortress::Resource
{
	class AnimationQueue
	{
	public:
		AnimationQueue() :
		m_set{},
		m_last_animation_code(INT_MAX),
		m_last_pop_duration(0),
		m_pop_elapsed(0.0f) {}

		void push(const int priority, const std::weak_ptr<GifWrapper>& animation);
		bool empty() const;
		std::weak_ptr<GifWrapper> update();

	private:
		using AnimationPair = std::pair<int, std::weak_ptr<GifWrapper>>;

		struct animation_sorting
		{
			bool operator()(
				const AnimationPair& left,	const AnimationPair& right) const
			{
				return left.first < right.first;
			}
		};

		// please note that this uses set, not queue.
		std::set<AnimationPair, animation_sorting> m_set;

		int m_last_animation_code;
		float m_last_pop_duration;
		float m_pop_elapsed;
	};

	inline void AnimationQueue::push(
		const int priority,
		const std::weak_ptr<GifWrapper>& animation)
	{
		m_set.emplace(priority, animation);
	}

	inline bool AnimationQueue::empty() const
	{
		return m_set.empty();
	}

	inline std::weak_ptr<GifWrapper> AnimationQueue::update()
	{
		m_pop_elapsed += DeltaTime::get_deltaTime();

		if(const auto iter = m_set.begin(); iter != m_set.end())
		{
			const auto next = *iter;
			const bool is_override = next.first < 0;
			// equal also checks due to different offset case (e.g., move left vs move right).
			const bool is_priority_high = next.first <= m_last_animation_code;
			const bool is_animation_finished = m_pop_elapsed >= m_last_pop_duration;

			if(is_override || is_animation_finished || is_priority_high)
			{
				m_last_animation_code = next.first;
				m_last_pop_duration = static_cast<float>(next.second.lock()->get_total_play_time()) / 1000;
				m_pop_elapsed = 0.0f;
				m_set.erase(m_set.begin());
				return next.second;
			}
		}

		return {};
	}
}
#endif // ANIMATIONQUEUE_HPP
