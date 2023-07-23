#ifndef ROUND_HPP
#define ROUND_HPP
#include <memory>

#include "character.hpp"
#include "deltatime.hpp"

namespace Fortress
{
	class Round
	{
	public:
		constexpr float timeout = 60.0f;
		void initialize();
		explicit Round(const std::vector<std::weak_ptr<ObjectBase::character>>& players);
		void check_countdown();

	private:
		float get_current_time() const;
		void next_player();

		float m_curr_timeout;
		float m_max_time = timeout;

		// here used vector instead of queue due to un-iterable.
		std::vector<std::weak_ptr<ObjectBase::character>> m_known_players;
		std::weak_ptr<ObjectBase::character> m_current_player;
	};

	inline void Round::initialize()
	{
		for(const auto& ptr : m_known_players)
		{
			if(const auto player = ptr.lock())
			{
				player->set_unmovable();
			}
		}

		m_curr_timeout = 0.0f;
		m_current_player = m_known_players.front();
		m_known_players.erase(m_known_players.begin());
	}

	inline Round::Round(const std::vector<std::weak_ptr<ObjectBase::character>>& players)
		: m_curr_timeout(0.0f)
	{
		m_known_players = players;
		initialize();
	}

	inline void Round::check_countdown()
	{
		m_curr_timeout += DeltaTime::get_deltaTime();

		if(get_current_time() - m_max_time < Math::epsilon)
		{
			next_player();
		}
	}

	inline float Round::get_current_time() const
	{
		return m_curr_timeout / 100;
	}

	inline void Round::next_player()
	{
		if(const auto player = m_current_player.lock())
		{
			player->set_unmovable();
		}

		m_known_players.push_back(m_current_player);
		m_current_player = m_known_players.front();
		m_known_players.erase(m_known_players.begin());

		if(const auto player = m_current_player.lock())
		{
			player->set_movable();
			const auto camera = Scene::SceneManager::get_active_scene().lock()->get_camera().lock();
			camera->set_object(m_current_player);
		}
	}
}
#endif // ROUND_HPP
