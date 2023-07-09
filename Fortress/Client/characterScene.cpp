#include "characterScene.h"
#include "CannonCharacter.hpp"
#include "input.hpp"
#include "winapihandles.hpp"

namespace Fortress::Scene
{
	void CharacterScene::initialize()
	{
		_scene::initialize();
		m_ground = std::make_shared<Object::Ground>();
		m_object = std::make_shared<Object::CannonCharacter>(
			Object::CannonCharacter(L"Ball", {1.0f, 1.0f}));

		m_object->m_bActive = false;
		m_ground->m_bActive = false;
	}

	void CharacterScene::update()
	{
		_scene::update();

		if (Input::getKey(eKeyCode::W))
		{
			m_object->move_up();
		}
		if (Input::getKey(eKeyCode::A))
		{
			m_object->move_left();
		}
		if (Input::getKey(eKeyCode::S))
		{
			m_object->move_down();
		}
		if (Input::getKey(eKeyCode::D))
		{
			m_object->move_right();
		}

		if (Input::getKeyDown(eKeyCode::SPACE))
		{
			m_object->shoot();
		}

		if (Input::getKeyUp(eKeyCode::W) ||
			Input::getKeyUp(eKeyCode::A) ||
			Input::getKeyUp(eKeyCode::S) ||
			Input::getKeyUp(eKeyCode::D))
		{
			m_object->stop();
		}

		ObjectBase::character::block_window_frame(*m_object);
	}

	void CharacterScene::render()
	{
		// @todo: Text and bar can be made in class
		// HP bar
		m_render_queue.push(0, [this]()
		{
			const int x = 30;
			const int y = 500;

			// hp text
			wchar_t hp_notice[100] = {};
			swprintf_s(hp_notice, 100, L"HP : ");
			const size_t strlen = wcsnlen_s(hp_notice, 100);
			TextOut(m_hdc, x, y + 5, hp_notice, strlen);

			// bar outline
			Rectangle(
				m_hdc,
				x + 20,
				y,
				x + 20 + 250,
				y + 25);

			// bar inside
			const HBRUSH brush = CreateSolidBrush(BLACK_BRUSH);
			const RECT rect = {x + 20, y, static_cast<int>(x + 20.0f + m_object->get_hp_percentage() * 250.0f), y + 25};
			FillRect(WinAPIHandles::get_buffer_dc(), &rect, brush);
			DeleteObject(brush);
		});

		// MP bar
		m_render_queue.push(0, [this]()
		{
			const int x = 30;
			const int y = 530;

			// hp text
			wchar_t hp_notice[100] = {};
			swprintf_s(hp_notice, 100, L"MP : ");
			const size_t strlen = wcsnlen_s(hp_notice, 100);
			TextOut(m_hdc, x, y + 5, hp_notice, strlen);

			// bar outline
			Rectangle(
				m_hdc,
				x + 20,
				y,
				x + 20 + 250,
				y + 25);

			// bar inside
			const HBRUSH brush = CreateSolidBrush(BLACK_BRUSH);
			const RECT rect = {x + 20, y, static_cast<int>(x + 20 + m_object->get_mp_percentage() * 250), y + 25};
			FillRect(WinAPIHandles::get_buffer_dc(), &rect, brush);
			DeleteObject(brush);
		});

		// Ground
		// @todo: workaround version of passing this pointer
		m_render_queue.push(0, [this]()
		{
			m_ground->render();
		});

		// Player
		m_render_queue.push(0, [this]()
		{
			m_object->render();
		});

		m_render_queue.push(1, [this]()
		{
			wchar_t notice[100] = {};
			swprintf_s(notice, 100, L"Use WASD to move...");
			const size_t strlen = wcsnlen_s(notice, 100);
			TextOut(m_hdc, 300, 300, notice, strlen);
		});

		_scene::render();
	}

	void CharacterScene::deactivate()
	{
		_scene::deactivate();

		m_object->m_bActive = false;
		m_ground->m_bActive = false;
	}

	void CharacterScene::activate()
	{
		m_object->m_bActive = true;
		m_ground->m_bActive = true;
	}
}
