#include "RoomScene.h"

#include "BulletinBoardScene.h"

#include "resourceManager.hpp"

void Fortress::Scene::RoomScene::initialize()
{
	scene::initialize();
	Resource::ResourceManager::load<ImageWrapper>(
		L"Room", "./resources/misc/room/room_blueprint.jpg");
	m_imBackground = Resource::ResourceManager::Find<ImageWrapper>(L"Room");
}

void Fortress::Scene::RoomScene::update()
{
	scene::update();
}

void Fortress::Scene::RoomScene::render()
{
	scene::render();

	m_imBackground->render({0, 0}, {1.0f, 0.97f});
}

void Fortress::Scene::RoomScene::deactivate()
{
	scene::deactivate();
}

void Fortress::Scene::RoomScene::activate()
{
	scene::activate();
}
