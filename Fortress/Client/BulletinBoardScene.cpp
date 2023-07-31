#include "BulletinBoardScene.h"

#include "input.hpp"
#include "resourceManager.hpp"

void Fortress::Scene::BulletinBoardScene::initialize()
{
	scene::initialize();
	Resource::ResourceManager::load<ImageWrapper>(
		L"Bulletin Board", "./resources/misc/bulletin_board/lobby_blueprint.png");
	m_imBackground = Resource::ResourceManager::find<ImageWrapper>(L"Bulletin Board");
	m_sound_bgm = Resource::ResourceManager::load<Resource::Sound>(
		L"Bulletin Board BGM", "./resources/sounds/notice_board.wav");
}

void Fortress::Scene::BulletinBoardScene::update()
{
	scene::update();

	if (Input::getKey(eKeyCode::SPACE))
	{
		SceneManager::SetActive(L"Lobby Scene");
	}
}

void Fortress::Scene::BulletinBoardScene::render()
{
	scene::render();

	m_imBackground.lock()->render({0, -20.f}, m_imBackground.lock()->get_hitbox());
}

void Fortress::Scene::BulletinBoardScene::deactivate()
{
	scene::deactivate();
	m_sound_bgm.lock()->stop(true);
}

void Fortress::Scene::BulletinBoardScene::activate()
{
	scene::activate();
	m_sound_bgm.lock()->play(true);
}
