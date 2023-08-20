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

template <typename T>
void Fortress::Scene::RoomScene::load_and_sync_map(const Network::GameInitMsg& game_info) const
{
	SceneManager::CreateScene<LoadingScene<T>>(game_info);
	Application::m_messenger.call_loading_finished(m_room_id);

	Debug::Log(L"Waiting for other clients...");

	static Network::GameStartMsg gsm{};

	if(gsm.type != Network::eMessageType::GameStart)
	{
		Application::m_messenger.check_game_start(gsm);
	}

	static float fixed_frame_wait = 0.0f;

	if (fixed_frame_wait >= 20.0f)
	{
		fixed_frame_wait = 0.0f;
		SceneManager::SetActive<LoadingScene<T>>();
		gsm = {};
	}

	fixed_frame_wait += DeltaTime::get_deltaTime();
}

void Fortress::Scene::RoomScene::update()
{
	scene::update();
	Network::GameInitMsg gis{};

	if (Input::getKey(eKeyCode::S))
	{
		Application::m_messenger.start_game(
			m_room_id, Network::eMapType::SkyValleyMap, &gis);
		Application::m_messenger.go_and_wait(m_room_id, gis.crc32);
		load_and_sync_map<Map::SkyValleyMap>(gis);
	}
	if (Input::getKey(eKeyCode::D))
	{
		Application::m_messenger.start_game(
			m_room_id, Network::eMapType::DesertMap, &gis);
		Application::m_messenger.go_and_wait(m_room_id, gis.crc32);
		load_and_sync_map<Map::DesertMap>(gis);
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
		if(Application::m_messenger.check_room_start(&gis))
		{
			switch(gis.map_type)
			{
			case Network::eMapType::DesertMap:
				Application::m_messenger.go_and_wait(m_room_id, gis.crc32);
				load_and_sync_map<Map::DesertMap>(gis);
				break;
			case Network::eMapType::SkyValleyMap: 
				Application::m_messenger.go_and_wait(m_room_id, gis.crc32);
				load_and_sync_map<Map::SkyValleyMap>(gis);
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
