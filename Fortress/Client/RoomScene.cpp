#include "RoomScene.h"

#include "application.h"
#include "BulletinBoardScene.h"
#include "DesertMap.hpp"
#include "../Common/input.hpp"
#include "LoadingScene.hpp"

#include "../Common/resourceManager.hpp"
#include "SkyValleyMap.hpp"

void Fortress::Scene::RoomScene::initialize()
{
	scene::initialize();
	Resource::ResourceManager::load<ImageWrapper>(
		L"Room", "./resources/misc/room/room_blueprint.jpg");
	m_imBackground = Resource::ResourceManager::find<ImageWrapper>(L"Room");
	m_bgm = Resource::ResourceManager::load<Resource::Sound>(
		L"Room BGM", "./resources/sounds/room.wav");
}

void Fortress::Scene::RoomScene::update()
{
	scene::update();
	Network::GameInitMsg game_info{};

	if (Input::getKey(eKeyCode::S))
	{
		Application::m_messenger.start_game(
			m_room_id, Network::eMapType::SkyValleyMap, &game_info);
		SceneManager::CreateScene<LoadingScene<Map::SkyValleyMap>>(game_info);
		Application::m_messenger.call_loading_finished(m_room_id);
		SceneManager::SetActive<LoadingScene<Map::SkyValleyMap>>();
	}
	if (Input::getKey(eKeyCode::D))
	{
		Application::m_messenger.start_game(
			m_room_id, Network::eMapType::DesertMap, &game_info);
		SceneManager::CreateScene<LoadingScene<Map::DesertMap>>(game_info);
		Application::m_messenger.call_loading_finished(m_room_id);
		SceneManager::SetActive<LoadingScene<Map::DesertMap>>();
	}
	if (Input::getKeyDown(eKeyCode::One))
	{
		Application::m_messenger.send_character(
			m_room_id, Network::eCharacterType::CannonCharacter);
	}
	if (Input::getKeyDown(eKeyCode::Two))
	{
		Application::m_messenger.send_character(
			m_room_id, Network::eCharacterType::MissileCharacter);
	}
	if (Input::getKeyDown(eKeyCode::Three))
	{
		Application::m_messenger.send_character(
			m_room_id, Network::eCharacterType::SecwindCharacter);
	}

	static float room_update = 0.0f;

	if(room_update >= 1.0f)
	{
		Application::m_messenger.check_room_update(&m_room_info);
		room_update = 0.0f;
	}

	room_update += DeltaTime::get_deltaTime();

	static float game_update = 0.0f;

	if(game_update >= 0.2f)
	{
		if(Application::m_messenger.check_room_start(&game_info))
		{
			switch(game_info.map_type)
			{
			case Network::eMapType::DesertMap:
				Application::m_messenger.send_confirm(m_room_id, game_info.crc32);
				SceneManager::CreateScene<LoadingScene<Map::DesertMap>>(game_info);
				Application::m_messenger.call_loading_finished(m_room_id);
				SceneManager::SetActive<LoadingScene<Map::DesertMap>>();
				break;
			case Network::eMapType::SkyValleyMap: 
				Application::m_messenger.send_confirm(m_room_id, game_info.crc32);
				SceneManager::CreateScene<LoadingScene<Map::SkyValleyMap>>(game_info);
				Application::m_messenger.call_loading_finished(m_room_id);
				SceneManager::SetActive<LoadingScene<Map::SkyValleyMap>>();
				break;
			default: break;
			}
		}
		
		game_update = 0.0f;
	}

	game_update += DeltaTime::get_deltaTime();

	for(const auto& player_name : m_room_info.player_names)
	{
		if(wcslen(player_name) != 0)
		{
			Debug::Log(player_name);
		}
	}
}

void Fortress::Scene::RoomScene::render()
{
	scene::render();

	m_imBackground.lock()->render({0, -20.f}, m_imBackground.lock()->get_hitbox());
}

void Fortress::Scene::RoomScene::deactivate()
{
	scene::deactivate();
	m_bgm.lock()->stop(true);
}

void Fortress::Scene::RoomScene::activate()
{
	Application::m_messenger.join_room(m_room_id, &m_room_info);
	scene::activate();
	m_bgm.lock()->play(true);
}
