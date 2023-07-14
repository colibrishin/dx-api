#include "BulletinBoardScene.h"

#include "resourceManager.hpp"

void Fortress::Scene::BulletinBoardScene::initialize()
{
	scene::initialize();
	Resource::ResourceManager::load<ImageWrapper>(
		L"Bulletin Board", "./resources/misc/bulletin_board/lobby_blueprint.png");
	m_imBackground = Resource::ResourceManager::Find<ImageWrapper>(L"Bulletin Board");
}

void Fortress::Scene::BulletinBoardScene::update()
{
	scene::update();
}

void Fortress::Scene::BulletinBoardScene::render()
{
	scene::render();

	m_imBackground->render({0, 0}, {1.0f, 0.97f});
}

void Fortress::Scene::BulletinBoardScene::deactivate()
{
	scene::deactivate();
}

void Fortress::Scene::BulletinBoardScene::activate()
{
	scene::activate();
}
