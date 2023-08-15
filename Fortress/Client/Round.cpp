#include "Round.h"

#include "BattleScene.h"
#include "projectile.hpp"
#include "SummaryScene.hpp"
#include "sceneManager.hpp"

namespace Fortress
{
	Round::Round(): m_state(eRoundState::Start)
	{
		
	}

	void Round::initialize(const std::vector<std::weak_ptr<ObjectBase::character>>& players)
	{
		for(const auto& ptr : players)
		{
			if(const auto player = ptr.lock())
			{
				player->set_unmovable();
			}
		}

		m_wind_affect = dis(e);
		m_state = eRoundState::InProgress;
		m_known_players = players;
		m_all_players = players;
		m_curr_timeout = 0.0f;
		m_current_player = m_known_players.front();
		m_known_players.erase(m_known_players.begin());
		m_timer_next_player = ObjectBase::TimerManager::create<NextPlayerTimer>(&Round::next_player, this);

		m_current_player.lock()->set_movable();
	}

	void Round::check_countdown()
	{
		if(get_current_time() >= max_time)
		{
			pre_next_player();
			m_curr_timeout = 0.0f;
		}

		if(!m_bfired)
		{
			m_curr_timeout += DeltaTime::get_deltaTime();
		}
	}

	void Round::check_fired()
	{
		if(const auto current = m_current_player.lock())
		{
			const auto scene = Scene::SceneManager::get_active_scene().lock();
			const auto camera = scene->get_camera().lock();

			if(current->get_state() == eCharacterState::Fired || 
				current->get_state() == eCharacterState::Item ||
				current->get_state() == eCharacterState::TurnEnd)
			{
				if(const auto prj = current->get_one_active_projectile().lock())
				{
					camera->set_object(prj);
				}

				m_state = eRoundState::Waiting;
			}
		}
	}

	void Round::check_explosion()
	{
		if(const auto current = m_current_player.lock())
		{
			const auto scene = Scene::SceneManager::get_active_scene().lock();
			const auto camera = scene->get_camera().lock();

			if(const auto prj = current->get_one_active_projectile().lock())
			{
				camera->set_object(prj);
			}

			if(!current->is_projectile_active() && 
				current->is_projectile_fire_counted() && 
				(current->get_state() == eCharacterState::Idle || current->get_state() == eCharacterState::Dead))
			{
				pre_next_player();
			}
		}
	}

	void Round::pre_next_player()
	{
		m_state = eRoundState::NextTurn;

		if(const auto player = m_current_player.lock())
		{
			player->set_unmovable();
		}

		if(m_timer_next_player.lock()->is_started())
		{
			return;
		}

		m_timer_next_player.lock()->toggle();
	}

	void Round::update()
	{
		Debug::Log(std::to_wstring(get_wind_acceleration()));

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
		case eRoundState::Waiting: 
			check_explosion();
			break;
		case eRoundState::NextTurn:
			check_winning_condition();
			break;
		case eRoundState::End:
			Debug::Log(m_winner.lock()->get_name() + L" won the match!");
			Scene::SceneManager::CreateScene<Scene::SummaryScene>(shared_from_this());
			Scene::SceneManager::SetActive(L"Summary Scene");
			Scene::SceneManager::remove_scene<Scene::BattleScene>();
			break;
		default: break;
		}
	}

	float Round::get_current_time() const
	{
		return m_curr_timeout;
	}

	// this will run on another thread, by win api.
	void Round::next_player()
	{
		const auto scene = Scene::SceneManager::get_active_scene().lock();
		const auto camera = scene->get_camera().lock();

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
			m_curr_timeout = 0.0f;
		}


		if(const auto player = m_current_player.lock())
		{
			player->set_movable();
			camera->set_object(m_current_player);
		}

		m_wind_affect = dis(e);
		m_state = eRoundState::InProgress;
		m_timer_next_player.lock()->stop();
	}

	void Round::check_winning_condition()
	{
		size_t count = 0;
		std::weak_ptr<ObjectBase::character> alive_one;

		for(const auto& alive : m_all_players)
		{
			if(alive.lock()->get_state() != eCharacterState::Dead)
			{
				count++;
				alive_one = alive;
			}
		}

		if(count == 1)
		{
			alive_one.lock()->set_unmovable();
			m_winner = alive_one;
			m_state = eRoundState::End;
			return;
		}

		if(m_state == eRoundState::NextTurn)
		{
			m_state = eRoundState::InProgress;
		}
	}

	const std::weak_ptr<ObjectBase::character>& Round::get_current_player() const
	{
		return m_current_player;
	}

	const eRoundState& Round::get_current_status() const
	{
		return m_state;
	}

	float Round::get_wind_acceleration() const
	{
		return static_cast<int>(m_wind_affect / 10.0f) * 10.0f;
	}
}

