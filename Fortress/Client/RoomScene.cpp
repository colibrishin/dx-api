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

	if (Input::getKey(eKeyCode::S))
	{
		SceneManager::CreateScene<LoadingScene<Map::SkyValleyMap>>();
		SceneManager::SetActive<LoadingScene<Map::SkyValleyMap>>();
	}
	if (Input::getKey(eKeyCode::D))
	{
		SceneManager::CreateScene<LoadingScene<Map::DesertMap>>();
		SceneManager::SetActive<LoadingScene<Map::DesertMap>>();
	}

	static float room_update = 0.0f;

	if(room_update >= 1.0f)
	{
		Application::m_messenger.check_room_update(&m_room_info);
		room_update = 0.0f;
	}

	room_update += DeltaTime::get_deltaTime();

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
