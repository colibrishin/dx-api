#include "titleScene.h"

#include <random>

#include "application.h"
#include "MissileCharacter.hpp"
#include "ImageWrapper.hpp"
#include "input.hpp"
#include "resourceManager.hpp"
#include "sceneManager.hpp"
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

		if (Input::getKey(eKeyCode::SPACE))
		{
			SceneManager::SetActive(L"Bulletin Board Scene");
		}
	}

	void TitleScene::render()
	{
		if(const auto camera = SceneManager::get_active_scene().lock()->get_camera().lock())
		{
			m_imBackground.lock()->render({}, m_imBackground.lock()->get_hitbox(), {1.0f, 1.0f});
		}

		[this]()
		{
			wchar_t notice[100] = {};
			swprintf_s(notice, 100, L"Press SPACE to continue...");
			const size_t strlen = wcsnlen_s(notice, 100);
			TextOut(WinAPIHandles::get_buffer_dc(), 300, 300, notice, strlen);
		}();

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
