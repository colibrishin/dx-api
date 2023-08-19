#include "titleScene.h"

#include <random>

#include "application.h"
#include "MissileCharacter.hpp"
#include "../Common/ImageWrapper.hpp"
#include "../Common/input.hpp"
#include "../Common/resourceManager.hpp"
#include "../Common/sceneManager.hpp"
#include "winapihandles.hpp"

namespace Fortress::Scene
{
	void TitleScene::initialize()
	{
		scene::initialize();

		Resource::ResourceManager::load<ImageWrapper>(L"Background", "./resources/images/background.jpg");
		m_imBackground = Resource::ResourceManager::find<ImageWrapper>(L"Background");
	}

	void TitleScene::update()
	{
		scene::update();

		if (Input::getKey(eKeyCode::One))
		{
			SceneManager::SetActive(L"Bulletin Board Scene");
			Application::m_messenger.set_player_id(1);
		}
		if (Input::getKey(eKeyCode::Two))
		{
			SceneManager::SetActive(L"Bulletin Board Scene");
			Application::m_messenger.set_player_id(2);
		}
	}

	void TitleScene::render()
	{
		if(const auto camera = SceneManager::get_active_scene().lock()->get_camera().lock())
		{
			m_imBackground.lock()->render({}, m_imBackground.lock()->get_hitbox(), {1.0f, 1.0f});
		}

		scene::render();
	}

	void TitleScene::deactivate()
	{
		scene::deactivate();
	}

	void TitleScene::activate()
	{
		scene::activate();
	}
}
