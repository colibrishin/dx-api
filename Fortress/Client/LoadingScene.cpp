#include "LobbyScene.h"

#include "resourceManager.hpp"

void Fortress::Scene::LobbyScene::initialize()
{
	scene::initialize();
	Resource::ResourceManager::load<ImageWrapper>(
		L"Loading", "./resources/misc/loading/loading.png");
	m_imBackground = Resource::ResourceManager::Find<ImageWrapper>(L"Loading");
}

void Fortress::Scene::LobbyScene::update()
{
	scene::update();
}

void Fortress::Scene::LobbyScene::render()
{
	scene::render();

	m_imBackground->render({0, 0}, {1.0f, 0.97f});
}

void Fortress::Scene::LobbyScene::deactivate()
{
	scene::deactivate();
}

void Fortress::Scene::LobbyScene::activate()
{
	scene::activate();
}
