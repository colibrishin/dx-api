#include "BattleScene.h"

#include "camera.hpp"
#include "MissileCharacter.hpp"
#include "ground.hpp"
#include "input.hpp"
#include "winapihandles.hpp"

#include <windows.h>
#include <MMSystem.h>

#include "objectManager.hpp"

#pragma comment (lib,"winmm.lib")

namespace Fortress::Scene
{
	void BattleScene::initialize()
	{
		scene::initialize();
		m_ground = ObjectBase::ObjectManager::create_object<Object::Ground>();
		m_object = ObjectBase::ObjectManager::create_object<Object::MissileCharacter>(
			L"Missile", Math::Vector2{1.0f, 1.0f}, Math::left);

		add_game_object(Abstract::LayerType::Character, m_object);
		add_game_object(Abstract::LayerType::Ground, m_ground);

		get_camera().lock()->set_object(m_object);

		m_object.lock()->set_disabled();
		m_ground.lock()->set_disabled();
	}

	void BattleScene::update()
	{
		scene::update();

		if (Input::getKey(eKeyCode::W))
		{
			m_object.lock()->move_up();
		}
		if (Input::getKey(eKeyCode::A))
		{
			m_object.lock()->move_left();
		}
		if (Input::getKey(eKeyCode::S))
		{
			m_object.lock()->move_down();
		}
		if (Input::getKey(eKeyCode::D))
		{
			m_object.lock()->move_right();
		}

		if(Input::getKey(eKeyCode::SPACE))
		{
			m_object.lock()->firing();
		}

		if(Input::getKeyUp(eKeyCode::SPACE))
		{
			m_object.lock()->shoot();
		}

		if (Input::getKeyUp(eKeyCode::W) ||
			Input::getKeyUp(eKeyCode::A) ||
			Input::getKeyUp(eKeyCode::S) ||
			Input::getKeyUp(eKeyCode::D))
		{
			m_object.lock()->stop();
		}
	}

	void BattleScene::render()
	{
		// @todo: Text and bar can be made in class
		// HP bar
		[this]()
		{
			const int x = 30;
			const int y = 500;

			// hp text
			wchar_t hp_notice[100] = {};
			swprintf_s(hp_notice, 100, L"HP : ");
			const size_t strlen = wcsnlen_s(hp_notice, 100);
			TextOut(WinAPIHandles::get_buffer_dc(), x, y + 5, hp_notice, strlen);

			// bar outline
			Rectangle(
				WinAPIHandles::get_buffer_dc(),
				x + 20,
				y,
				x + 20 + 250,
				y + 25);

			// bar inside
			const HBRUSH brush = CreateSolidBrush(BLACK_BRUSH);
			const RECT rect = {x + 20, y, static_cast<int>(x + 20.0f + m_object.lock()->get_hp_percentage() * 250.0f), y + 25};
			FillRect(WinAPIHandles::get_buffer_dc(), &rect, brush);
			DeleteObject(brush);
		}();

		// MP bar
		[this]()
		{
			const int x = 30;
			const int y = 530;

			// hp text
			wchar_t hp_notice[100] = {};
			swprintf_s(hp_notice, 100, L"MP : ");
			const size_t strlen = wcsnlen_s(hp_notice, 100);
			TextOut(WinAPIHandles::get_buffer_dc(), x, y + 5, hp_notice, strlen);

			// bar outline
			Rectangle(
				WinAPIHandles::get_buffer_dc(),
				x + 20,
				y,
				x + 20 + 250,
				y + 25);

			// bar inside
			const HBRUSH brush = CreateSolidBrush(BLACK_BRUSH);
			const RECT rect = {x + 20, y, static_cast<int>(x + 20 + m_object.lock()->get_mp_percentage() * 250), y + 25};
			FillRect(WinAPIHandles::get_buffer_dc(), &rect, brush);
			DeleteObject(brush);
		}();

		[this]()
		{
			wchar_t notice[100] = {};
			swprintf_s(notice, 100, L"Use WASD to move...");
			const size_t strlen = wcsnlen_s(notice, 100);
			TextOut(WinAPIHandles::get_buffer_dc(), 300, 300, notice, strlen);
		}();

		scene::render();
	}

	void BattleScene::deactivate()
	{
		scene::deactivate();
	}

	void BattleScene::activate()
	{
		scene::activate();
	}
}
