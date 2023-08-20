#include "LobbyScene.h"

#include "application.h"
#include "../Common/input.hpp"
#include "../Common/resourceManager.hpp"
#include "../Common/debug.hpp"

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

	static float lobby_update = 0.0f;

	if(lobby_update >= 1.0f)
	{
		Application::m_messenger.check_lobby_update(&m_lobby_info_);
		lobby_update = 0.0f;
	}

	lobby_update += DeltaTime::get_deltaTime();
}

void Fortress::Scene::LobbyScene::render()
{
	scene::render();

	m_imBackground.lock()->render({0, -20.f}, m_imBackground.lock()->get_hitbox());

	for(const auto& name : m_lobby_info_.player_names)
	{
		if(wcslen(name) != 0)
		{
			Debug::Log(name);
		}
	}
}

void Fortress::Scene::LobbyScene::deactivate()
{
	scene::deactivate();
	m_bgm.lock()->stop(true);
}

void Fortress::Scene::LobbyScene::activate()
{
	Application::m_messenger.join_lobby(&m_lobby_info_);
	scene::activate();
	m_bgm.lock()->play(true);
}
