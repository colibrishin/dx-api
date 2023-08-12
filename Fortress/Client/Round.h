#ifndef ROUND_HPP
#define ROUND_HPP
#include <memory>
#include <random>
#include "deltatime.hpp"
#include "NextPlayerTimer.hpp"

namespace Fortress
{
	namespace ObjectBase
	{
		class character;
	}

	constexpr float max_time = 60.0f;

	class Round : public std::enable_shared_from_this<Round>
	{
	public:
		Round() : m_state(eRoundState::Start) {}
		void initialize(const std::vector<std::weak_ptr<ObjectBase::character>>& players);
		void update();
		float get_current_time() const;
		float get_wind_acceleration() const;
		const std::weak_ptr<ObjectBase::character>& get_current_player() const;
		const eRoundState& get_current_status() const;

	private:
		void check_countdown();
		void check_fired();
		void pre_next_player();
		void next_player();
		void check_winning_condition();
		void winner();

		float m_curr_timeout = 0.0f;
		bool m_bfired = false;

		std::weak_ptr<NextPlayerTimer> m_timer_next_player;

		float m_wind_affect = 0.0f;
		// @todo: random seed should be different every round
		inline static std::default_random_engine e;
		inline static std::uniform_real_distribution<float> dis{-50, 50};

		// here used vector instead of queue due to un-iterable.
		eRoundState m_state;
		std::vector<std::weak_ptr<ObjectBase::character>> m_known_players;
		std::vector<std::weak_ptr<ObjectBase::character>> m_all_players;
		std::weak_ptr<ObjectBase::character> m_current_player;
		std::weak_ptr<ObjectBase::character> m_winner;
	};
}
#endif // ROUND_HPP
