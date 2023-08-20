#ifndef MAP_HPP
#define MAP_HPP
#include "application.h"
#include "../Common/BattleScene.h"
#include "CannonCharacter.hpp"
#include "EgyptGround.hpp"
#include "EgyptStairway.hpp"
#include "MissileCharacter.hpp"
#include "../Common/objectManager.hpp"
#include "SecwindCharacter.hpp"

namespace Fortress::Map
{
	class Map : public Scene::BattleScene
	{
	public:
		Map(const std::wstring& name, const Network::GameInitMsg& game_init) : BattleScene(name, game_init)
		{
		}
		void initialize() override;
		void pre_initialize() override;
		void set_bgm() override;
		void set_background_img() override;
		void set_grounds() override;

	private:
		void set_characters() override;
		void set_client_character() override;
	};

	inline void Map::initialize()
	{
		BattleScene::initialize();
	}

	inline void Map::pre_initialize()
	{
	}

	inline void Map::set_bgm()
	{
	}

	inline void Map::set_background_img()
	{
	}

	inline void Map::set_characters()
	{
		for(int i = 0; i < 15; ++i)
		{
			if(m_game_init.players[i] == 0)
			{
				continue;
			}

			switch(m_game_init.character_type[i])
			{
			case Network::eCharacterType::CannonCharacter: 
				m_characters.emplace_back(
				ObjectBase::ObjectManager::create_object<Object::CannonCharacter>(
				m_game_init.players[i], L"Cannon" + std::to_wstring(m_game_init.players[i]), Math::Vector2{-1000, 0}, Math::left));
				break;
			case Network::eCharacterType::MissileCharacter: 
				m_characters.emplace_back(
				ObjectBase::ObjectManager::create_object<Object::MissileCharacter>(
				m_game_init.players[i], L"Missile" + std::to_wstring(m_game_init.players[i]), Math::Vector2{-1000, 0}, Math::left));
				break;
			case Network::eCharacterType::SecwindCharacter: 
				m_characters.emplace_back(
				ObjectBase::ObjectManager::create_object<Object::SecwindCharacter>(
				m_game_init.players[i], L"Secwind" + std::to_wstring(m_game_init.players[i]), Math::Vector2{-1000, 0}, Math::left));
				break;
			case Network::eCharacterType::None: break;
			default: ;
			}
		}
	}

	inline void Map::set_client_character()
	{
		m_self = *std::find_if(
			m_characters.begin(), 
			m_characters.end(),
			[](const std::weak_ptr<ObjectBase::character>& ch)
			{
				// server = -1, reserved = 0
				return ch.lock()->get_player_id() == Application::m_messenger.get_player_id();
			});
	}

	inline void Map::set_grounds()
	{
	}
}
#endif // DESERTMAP_HPP
