#include "LobbyScene.h"

#include "resourceManager.hpp"

void Fortress::Scene::LobbyScene::initialize()
{
	scene::initialize();
	Resource::ResourceManager::load<ImageWrapper>(
		L"Lobby", "./resources/misc/lobby/lobby_blueprint.png");
	m_imBackground = Resource::ResourceManager::find<ImageWrapper>(L"Lobby");
}

void Fortress::Scene::LobbyScene::update()
{
	scene::update();
}

void Fortress::Scene::LobbyScene::render()
{
	scene::render();

	m_imBackground.lock()->render({0, 0}, {1.0f, 0.97f});
}

void Fortress::Scene::LobbyScene::deactivate()
{
	scene::deactivate();
}

void Fortress::Scene::LobbyScene::activate()
{
	scene::activate();
}
