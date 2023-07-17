#include "LoadingScene.h"

#include "resourceManager.hpp"

void Fortress::Scene::LoadingScene::initialize()
{
	scene::initialize();
	Resource::ResourceManager::load<ImageWrapper>(
		L"Loading", "./resources/misc/loading/loading.png");
	m_imBackground = Resource::ResourceManager::find<ImageWrapper>(L"Loading");
}

void Fortress::Scene::LoadingScene::update()
{
	scene::update();
}

void Fortress::Scene::LoadingScene::render()
{
	scene::render();

	m_imBackground.lock()->render({0, 0}, {1.0f, 0.97f});
}

void Fortress::Scene::LoadingScene::deactivate()
{
	scene::deactivate();
}

void Fortress::Scene::LoadingScene::activate()
{
	scene::activate();
}
