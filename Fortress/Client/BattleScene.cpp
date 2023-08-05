#include "BattleScene.h"

#include "camera.hpp"
#include "MissileCharacter.hpp"
#include "CannonCharacter.hpp"
#include "ground.hpp"
#include "input.hpp"
#include "winapihandles.hpp"

#include <windows.h>
#include "objectManager.hpp"


namespace Fortress::Scene
{
	void BattleScene::initialize()
	{
		scene::initialize();
		pre_initialize();

		m_hud = Resource::ResourceManager::load<ImageWrapper>(
			L"HUD", "./resources/images/hud.gif");
		set_grounds();
		set_background_img();
		set_bgm();
		set_characters();
		set_client_character();

		for (const auto& ch : m_characters)
		{
			add_game_object(Abstract::LayerType::Character, ch);
			ch.lock()->set_disabled();
		}

		for (const auto& gr : m_grounds)
		{
			add_game_object(Abstract::LayerType::Ground, gr);
			gr.lock()->set_disabled();
		}

		get_camera().lock()->set_object(m_self);
		m_round.initialize(m_characters);
		m_radar.initialize();
	}

	void BattleScene::update()
	{
		scene::update();
		m_round.update();
		m_radar.update();
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
					static_cast<int>(x + m_self.lock()->get_hp_percentage() * 400.0f),
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

			[this]()
			{
				const std::wstring time_remaining = std::to_wstring(
					static_cast<int>(max_time - m_round.get_current_time()));

				TextOut(
					WinAPIHandles::get_buffer_dc(), 
					750, 
					525, 
					time_remaining.c_str(),
					time_remaining.length());
			}();

			// Charged power
			static float cached_charged = m_self.lock()->get_charged_power();

			if(m_self.lock()->get_state() == eCharacterState::Firing)
			{
				cached_charged = m_self.lock()->get_charged_power();
			}

			[this, hud_position]()
			{
				const int x = 280;
				const int y = hud_position.get_y() + 74;
				
				// bar inside
				const HBRUSH brush = CreateSolidBrush(RGB(200,0,100));
				const RECT rect = {
					x,
					y,
					static_cast<int>(x + ((cached_charged / ObjectBase::character_max_charge) * 400.0f)),
					y + 20};
				FillRect(WinAPIHandles::get_buffer_dc(), &rect, brush);
				DeleteObject(brush);
			}();

			[this, hud_position]()
			{
				const int x = 280;
				const int y = hud_position.get_y() + 74;
				
				// bar inside
				const HBRUSH brush = CreateSolidBrush(RGB(255,0,0));
				const RECT rect = {
					x,
					y,
					static_cast<int>(x + ((m_self.lock()->get_charged_power() / ObjectBase::character_max_charge) * 400.0f)),
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
					static_cast<int>(x + m_self.lock()->get_mp_percentage() * 400.0f),
					y + 20};
				FillRect(WinAPIHandles::get_buffer_dc(), &rect, brush);
				DeleteObject(brush);
			}();
		}

		m_radar.render();
	}

	void BattleScene::deactivate()
	{
		scene::deactivate();
		m_bgm.lock()->stop(true);
	}

	void BattleScene::activate()
	{
		scene::activate();
		m_bgm.lock()->play(true);
	}

	const Round& BattleScene::get_round_status()
	{
		return m_round;
	}

	const Math::Vector2& BattleScene::get_map_size() const
	{
		return m_map_size;
	}
}
