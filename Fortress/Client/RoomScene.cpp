#include "RoomScene.h"

#include "BulletinBoardScene.h"
#include "input.hpp"

#include "resourceManager.hpp"

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

	if (Input::getKey(eKeyCode::SPACE))
	{
		SceneManager::SetActive(L"Character Scene");
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
