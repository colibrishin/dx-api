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
	SceneManager::SetActive<LoadingScene<T>>();
}

void Fortress::Scene::RoomScene::update()
{
	scene::update();
	Network::GameInitMsg gis{};

	if (Input::getKey(eKeyCode::S))
	{
		EngineHandle::get_messenger()->start_game(
			Network::eMapType::SkyValleyMap, &gis);
		load_and_sync_map<Map::SkyValleyMap>(gis);
		return;
	}
	if (Input::getKey(eKeyCode::D))
	{
		EngineHandle::get_messenger()->start_game(
			Network::eMapType::DesertMap, &gis);
		load_and_sync_map<Map::DesertMap>(gis);
		return;
	}
	if (Input::getKeyDown(eKeyCode::One))
	{
		EngineHandle::get_messenger()->send_character(
			Network::eCharacterType::CannonCharacter);
	}
	if (Input::getKeyDown(eKeyCode::Two))
	{
		EngineHandle::get_messenger()->send_character(
			Network::eCharacterType::MissileCharacter);
	}
	if (Input::getKeyDown(eKeyCode::Three))
	{
		EngineHandle::get_messenger()->send_character(
			Network::eCharacterType::SecwindCharacter);
	}

	static float room_update = 0.0f;

	if(room_update >= 1.0f)
	{
		EngineHandle::get_messenger()->check_room_update(&m_room_info);
		room_update = 0.0f;
	}

	room_update += DeltaTime::get_deltaTime();

	static float game_update = 0.0f;

	if(game_update >= 0.2f)
	{
		if(EngineHandle::get_messenger()->check_room_start(&gis))
		{
			switch(gis.map_type)
			{
			case Network::eMapType::DesertMap:
				load_and_sync_map<Map::DesertMap>(gis);
				break;
			case Network::eMapType::SkyValleyMap: 
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
	EngineHandle::get_messenger()->join_room(m_room_id, &m_room_info);
	scene::activate();
	m_bgm.lock()->play(true);
}
