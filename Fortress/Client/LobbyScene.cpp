#include "LobbyScene.h"

#include "input.hpp"
#include "resourceManager.hpp"

void Fortress::Scene::LobbyScene::initialize()
{
	scene::initialize();
	Resource::ResourceManager::load<ImageWrapper>(
		L"Lobby", "./resources/misc/lobby/lobby_blueprint.png");
	m_imBackground = Resource::ResourceManager::find<ImageWrapper>(L"Lobby");
	m_bgm = Resource::ResourceManager::load<Resource::Sound>(
		L"Lobby BGM", "./resources/sounds/lobby.wav");
}

void Fortress::Scene::LobbyScene::update()
{
	scene::update();

	if (Input::getKey(eKeyCode::SPACE))
	{
		SceneManager::SetActive(L"Room Scene");
	}
}

void Fortress::Scene::LobbyScene::render()
{
	scene::render();

	m_imBackground.lock()->render({0, -20.f}, m_imBackground.lock()->get_hitbox());
}

void Fortress::Scene::LobbyScene::deactivate()
{
	scene::deactivate();
	m_bgm.lock()->stop(true);
}

void Fortress::Scene::LobbyScene::activate()
{
	scene::activate();
	m_bgm.lock()->play(true);
}
