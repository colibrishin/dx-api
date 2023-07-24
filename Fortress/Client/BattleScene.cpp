#include "BattleScene.h"

#include "camera.hpp"
#include "MissileCharacter.hpp"
#include "CannonCharacter.hpp"
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
		m_hud = Resource::ResourceManager::load<ImageWrapper>(L"HUD", "./resources/images/hud.gif");
		m_background = Resource::ResourceManager::load<ImageWrapper>(
			L"valley", "./resources/images/wirestock_valley.jpg");
		m_ground = ObjectBase::ObjectManager::create_object<Object::Ground>();
		m_home_object = ObjectBase::ObjectManager::create_object<Object::MissileCharacter>(
			0, L"Missile", Math::Vector2{1.0f, 1.0f}, Math::right);
		m_away_object = ObjectBase::ObjectManager::create_object<Object::CannonCharacter>(
			1, L"Cannon", Math::Vector2{300.0f, 1.0f}, Math::left);

		add_game_object(Abstract::LayerType::Character, m_home_object);
		add_game_object(Abstract::LayerType::Character, m_away_object);
		add_game_object(Abstract::LayerType::Ground, m_ground);

		get_camera().lock()->set_object(m_home_object);

		m_home_object.lock()->set_disabled();
		m_ground.lock()->set_disabled();
	}

	void BattleScene::update()
	{
		scene::update();

		if (const auto obj = m_home_object.lock())
		{
			switch(obj->get_state())
			{
			case eCharacterState::Idle:
				obj->idle_state();
				break;
			case eCharacterState::Firing:
				obj->firing_state();
				break;
			case eCharacterState::Fire:
				obj->fire_state();
				break;
			case eCharacterState::Item:
				break;
			case eCharacterState::Move:
				obj->move_state();
				break;
			case eCharacterState::Falling:
				obj->falling_state();
				break;
			default:
				break;
			}
		}

		if (const auto obj = m_away_object.lock())
		{
			switch(obj->get_state())
			{
			case eCharacterState::Idle:
				obj->idle_state();
				break;
			case eCharacterState::Firing:
				obj->firing_state();
				break;
			case eCharacterState::Fire:
				obj->fire_state();
				break;
			case eCharacterState::Item:
				break;
			case eCharacterState::Move:
				obj->move_state();
				break;
			case eCharacterState::Falling:
				obj->falling_state();
				break;
			default:
				break;
			}
		}
	}

	void BattleScene::render()
	{
		m_background.lock()->render({}, m_background.lock()->get_hitbox());
		scene::render();

		if(const auto hud_ptr = m_hud.lock())
		{
			const auto hud_size = hud_ptr->get_hitbox();
			const auto hud_position = Math::Vector2{0, WinAPIHandles::get_actual_max_y() - hud_size.get_y()};
			hud_ptr->render({0, hud_position.get_y()}, hud_size);

			// @todo: Text and bar can be made in class
			// MP bar
			[this, hud_position]()
			{
				const int x = 280;
				const int y = hud_position.get_y() + 98;
				
				// bar inside
				const HBRUSH brush = CreateSolidBrush(RGB(255, 255,0));
				const RECT rect = {
					x,
					y,
					static_cast<int>(x + m_home_object.lock()->get_mp_percentage() * 400.0f),
					y + 20};
				FillRect(WinAPIHandles::get_buffer_dc(), &rect, brush);
				DeleteObject(brush);
			}();

			// HP bar
			[this, hud_position]()
			{
				const int x = 280;
				const int y = hud_position.get_y() + 52;
				
				// bar inside
				const HBRUSH brush = CreateSolidBrush(RGB(0,255,0));
				const RECT rect = {
					x,
					y,
					static_cast<int>(x + m_home_object.lock()->get_hp_percentage() * 400.0f),
					y + 20};
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
		}
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
