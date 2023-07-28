#ifndef ROUND_HPP
#define ROUND_HPP
#include <memory>

#include "character.hpp"
#include "deltatime.hpp"

namespace Fortress
{
	enum class eRoundState
	{
		Start = 0,
		InProgress,
		End,
	};

	class Round
	{
	public:
		Round() : m_state(eRoundState::Start) {}
		void initialize(const std::vector<std::weak_ptr<ObjectBase::character>>& players);
		void update();
		float get_current_time() const;

	private:
		void check_countdown();
		void check_fired();
		void next_player();
		void check_winning_condition();
		void winner();

		float m_curr_timeout = 0.0f;
		const float m_max_time = 60.0f;
		bool m_bfired = false;

		// here used vector instead of queue due to un-iterable.
		eRoundState m_state;
		std::vector<std::weak_ptr<ObjectBase::character>> m_known_players;
		std::weak_ptr<ObjectBase::character> m_current_player;
		std::weak_ptr<ObjectBase::character> m_winner;

		struct safe_weak_comparer {
		    bool operator() (const std::weak_ptr<ObjectBase::character> &lhs, const std::weak_ptr<ObjectBase::character> &rhs)const {
		        const auto lptr = lhs.lock();
		    	const auto rptr = rhs.lock();
		        if (!rptr) return false; // nothing after expired pointer 
		        if (!lptr) return true;  // every not expired after expired pointer
		        return lptr.get() < rptr.get();
		    }
		};
	};

	inline void Round::initialize(const std::vector<std::weak_ptr<ObjectBase::character>>& players)
	{
		for(const auto& ptr : m_known_players)
		{
			if(const auto player = ptr.lock())
			{
				player->set_unmovable();
			}
		}

		m_state = eRoundState::InProgress;
		m_known_players = players;
		m_curr_timeout = 0.0f;
		m_current_player = m_known_players.front();
		m_known_players.erase(m_known_players.begin());
	}

	inline void Round::check_countdown()
	{
		if(get_current_time() >= m_max_time)
		{
			next_player();
			m_curr_timeout = 0.0f;
		}

		m_curr_timeout += DeltaTime::get_deltaTime();
	}

	inline void Round::check_fired()
	{
		if(const auto current = m_current_player.lock())
		{
			if(current->get_state() == eCharacterState::Fire)
			{
				m_bfired = true;
			}
			if(current->get_state() == eCharacterState::Idle && m_bfired)
			{
				m_bfired = false;
				next_player();
			}
		}
	}

	inline void Round::update()
	{
		Debug::Log(std::to_wstring(m_curr_timeout));
		switch (m_state)
		{
		case eRoundState::Start:
			// @todo: scrolling to the map start to end.
			break;
		case eRoundState::InProgress:
			// @todo: arrow man
			check_fired();
			check_countdown();
			check_winning_condition();
			break;
		case eRoundState::End:
			Debug::Log(m_winner.lock()->get_name() + L" won the match!");
			// @todo: move to battle summary.
		default: break;
		}
	}

	inline float Round::get_current_time() const
	{
		return m_curr_timeout;
	}

	inline void Round::next_player()
	{
		if(const auto player = m_current_player.lock())
		{
			player->set_unmovable();
		}

		const auto previous_character = m_current_player;

		// @todo: if every player is dead except current player then this will run indefinitely.
		while (previous_character.lock().get() == m_current_player.lock().get())
		{
			m_known_players.push_back(m_current_player);

			if(m_known_players.front().lock()->get_state() == eCharacterState::Dead)
			{
				m_known_players.push_back(*m_known_players.begin());
				m_known_players.erase(m_known_players.begin());
				continue;
			}
			
			m_current_player = m_known_players.front();
			m_known_players.erase(m_known_players.begin());
		}


		if(const auto player = m_current_player.lock())
		{
			player->set_movable();
			const auto camera = Scene::SceneManager::get_active_scene().lock()->get_camera().lock();
			camera->set_object(m_current_player);
		}
	}

	inline void Round::check_winning_condition()
	{
		static std::set<std::weak_ptr<ObjectBase::character>, safe_weak_comparer> alive_characters;

		for(const auto& alive : m_known_players)
		{
			if(alive.lock()->get_state() != eCharacterState::Dead)
			{
				alive_characters.insert(alive);
			}
			else if(alive.lock()->get_state() == eCharacterState::Dead)
			{
				alive_characters.erase(alive);
			}
		}

		if(alive_characters.size() == 1)
		{
			(*alive_characters.begin()).lock()->set_unmovable();
			m_winner = *alive_characters.begin();
			m_state = eRoundState::End;
		}
	}
}
#endif // ROUND_HPP
