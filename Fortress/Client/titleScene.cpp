#include "titleScene.h"

#include <random>

#include "application.h"
#include "CannonCharacter.hpp"
#include "input.hpp"
#include "sceneManager.hpp"
#include "winapihandles.hpp"

namespace Fortress::Scene
{
	void TitleScene::initialize()
	{
		_scene::initialize();

		for (auto& m_object : m_objects)
		{
			static thread_local std::mt19937 generator(static_cast<unsigned int>(time(nullptr)));
			std::uniform_int_distribution h_distribution(0, WinAPIHandles::get_window_width());
			std::uniform_int_distribution w_distribution(0, WinAPIHandles::get_window_height());

			Math::Vector2 random_pos = {
				static_cast<float>(w_distribution(generator)),
				static_cast<float>(h_distribution(generator))
			};
			m_object = std::make_shared<Object::CannonCharacter>(
				Object::CannonCharacter{L"Ball", random_pos});

			m_object->m_velocity = {1.0f, 1.0f};
			m_object->m_bActive = false;
		}
	}

	void TitleScene::update()
	{
		_scene::update();
		for (auto& obj : m_objects)
		{
			ObjectBase::character::block_window_frame(*obj);
		}

		if (Input::getKey(eKeyCode::SPACE))
		{
			SceneManager::SetActive(L"Character Scene");
		}
	}

	void TitleScene::render()
	{
		m_render_queue.push(0, [this]()
		{
			for (const auto& m_object : m_objects)
			{
				Ellipse(
					m_hdc,
					m_object->get_x(),
					m_object->get_y(),
					m_object->get_x() + m_object->m_hitbox.get_x(),
					m_object->get_y() + m_object->m_hitbox.get_y());
			}
		});

		m_render_queue.push(1, [this]()
		{
			wchar_t notice[100] = {};
			swprintf_s(notice, 100, L"Press SPACE to continue...");
			const size_t strlen = wcsnlen_s(notice, 100);
			TextOut(m_hdc, 300, 300, notice, strlen);
		});

		_scene::render();
	}

	void TitleScene::deactivate()
	{
		for (const auto& ch : m_objects)
		{
			ch->m_bActive = false;
		}
	}

	void TitleScene::activate()
	{
		for (const auto& ch : m_objects)
		{
			ch->m_bActive = true;
		}
	}
}
