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

		static thread_local std::mt19937 generator(static_cast<unsigned int>(time(nullptr)));
		std::uniform_int_distribution h_distribution(0, WinAPIHandles::get_window_width());
		std::uniform_int_distribution w_distribution(0, WinAPIHandles::get_window_height());

		Math::Vector2 random_pos = {
			static_cast<float>(w_distribution(generator)),
			static_cast<float>(h_distribution(generator))
		};

		m_object = std::make_shared<Object::MissileCharacter>(0, L"Ball", random_pos, Math::left);

		add_game_object(Abstract::LayerType::Character, m_object);

		auto rb = std::dynamic_pointer_cast<Abstract::rigidBody>(get_objects().back().lock());

		rb->m_velocity = {1.0f, 1.0f};
		rb->set_disabled();
		get_camera().lock()->set_object(rb);
	}

	void TitleScene::update()
	{
		scene::update();

		if(auto rb = std::dynamic_pointer_cast<Abstract::rigidBody>(get_objects().back().lock()))
		{
			ObjectBase::character::block_window_frame(rb);
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

		m_imBackground.lock()->render({}, {});
		get_objects().back().lock()->render();

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
