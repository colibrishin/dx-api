#include "RoomScene.h"

#include "BulletinBoardScene.h"
#include "DesertMap.hpp"
#include "input.hpp"
#include "LoadingScene.hpp"

#include "resourceManager.hpp"
#include "SkyValleyMap.hpp"
#include "BattleScene.h"

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
	scene::activate();
	m_bgm.lock()->play(true);
}
