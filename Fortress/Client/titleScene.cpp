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
		m_imBackground = Resource::ResourceManager::Find<ImageWrapper>(L"Background");

		for (int i = 0; i < 1; ++i)
		{
			static thread_local std::mt19937 generator(static_cast<unsigned int>(time(nullptr)));
			std::uniform_int_distribution h_distribution(0, WinAPIHandles::get_window_width());
			std::uniform_int_distribution w_distribution(0, WinAPIHandles::get_window_height());

			Math::Vector2 random_pos = {
				static_cast<float>(w_distribution(generator)),
				static_cast<float>(h_distribution(generator))
			};
			add_game_object(Abstract::LayerType::Character, new Object::MissileCharacter{L"Ball", random_pos, Math::left});

			auto* rb = dynamic_cast<Abstract::rigidBody*>(m_objects.back());

			rb->m_velocity = {1.0f, 1.0f};
			rb->set_disabled();
			get_camera()->set_object(rb);
		}
	}

	void TitleScene::update()
	{
		scene::update();

		for (const auto obj : m_objects)
		{
			if(auto* rb = dynamic_cast<Abstract::rigidBody*>(obj))
			{
				ObjectBase::character::block_window_frame(rb);
			}
		}

		if (Input::getKey(eKeyCode::SPACE))
		{
			SceneManager::SetActive(L"Character Scene");
		}

		if (Input::getKey(eKeyCode::L))
		{
			SceneManager::SetActive(L"Lobby Scene");
		}

		if (Input::getKey(eKeyCode::B))
		{
			SceneManager::SetActive(L"Bulletin Board Scene");
		}

		if (Input::getKey(eKeyCode::R))
		{
			SceneManager::SetActive(L"Room Scene");
		}
	}

	void TitleScene::render()
	{
		[this]()
		{
			wchar_t notice[100] = {};
			swprintf_s(notice, 100, L"Press SPACE to continue...");
			const size_t strlen = wcsnlen_s(notice, 100);
			TextOut(WinAPIHandles::get_buffer_dc(), 300, 300, notice, strlen);
		}();

		m_imBackground->render({}, {});
		m_objects[0]->render();

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
