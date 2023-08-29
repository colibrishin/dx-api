#include "pch.h"
#include "BattleScene.h"

#include "camera.hpp"
#include "ground.hpp"
#include "input.hpp"

#include <windows.h>

#include "item.hpp"
#include "message.hpp"
#include "objectManager.hpp"
#include "Radar.h"

#undef min
#undef max

namespace Fortress::Scene
{
	BattleScene::BattleScene(const std::wstring& name, const Network::GameInitMsg& game_init):
		scene(L"Battle Scene " + name),
		m_map_size({}),
		m_round(std::make_shared<Round>()), // lazy-initialization
		m_game_init(game_init)
	{
	}

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
		spawnpoints();

		m_map_size = evaluate_map_size();
		m_radar = std::make_unique<Radar>(m_map_size);

		for (const auto& [pid, ch] : m_characters)
		{
			add_game_object(Abstract::LayerType::Character, ch);
			ch.lock()->set_disabled();
		}

		for (const auto& gr : m_grounds)
		{
			add_game_object(Abstract::LayerType::Ground, gr);
			gr.lock()->set_disabled();
		}

		get_camera().lock()->set_object(m_characters.begin()->second);
		m_round->initialize(get_characters());
		m_radar->initialize();
	}

	void BattleScene::update()
	{
		scene::update();
		m_round->update();
		m_radar->update();
	}

	Math::Vector2 operator/(float lhs, const Math::Vector2& rhs)
	{
		return {lhs / rhs.get_x(), lhs / rhs.get_y()};
	}

	void BattleScene::render()
	{
		m_background.lock()->render({}, m_background.lock()->get_hitbox());
		scene::render();

		if(const auto hud_ptr = m_hud.lock())
		{
			const auto hud_size = hud_ptr->get_hitbox();
			const auto hud_position = Math::Vector2{0, EngineHandle::get_handle().lock()->get_actual_max_y() - hud_size.get_y()};
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
				FillRect(EngineHandle::get_handle().lock()->get_buffer_dc(), &rect, brush);
				DeleteObject(brush);
			}();

			[this]()
			{
				const std::wstring time_remaining = std::to_wstring(
					static_cast<int>(max_time - m_round->get_current_time()));

				const SolidBrush yellow(Color(255, 255, 255, 0));

				EngineHandle::get_handle().lock()->get_buffer_gdi_handle().lock()->DrawString(
					time_remaining.c_str(),
					time_remaining.length(),
					EngineHandle::get_handle().lock()->get_font().lock().get(),
					PointF
					{
						720.f,
						495.f
					},
					&yellow);
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
				FillRect(EngineHandle::get_handle().lock()->get_buffer_dc(), &rect, brush);
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
				FillRect(EngineHandle::get_handle().lock()->get_buffer_dc(), &rect, brush);
				DeleteObject(brush);
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
				FillRect(EngineHandle::get_handle().lock()->get_buffer_dc(), &rect, brush);
				DeleteObject(brush);
			}();

			const auto map = SceneManager::get_active_map().lock();
			const auto wind = map->get_round_status().lock()->get_wind_acceleration();
			constexpr float max_wind = 50.0f;

			// wind bar
			[this, hud_position, wind]()
			{
				const int x = 90;
				const int y = hud_position.get_y() + 112;
				
				// bar inside
				const HBRUSH brush = CreateSolidBrush(RGB(0, 255,0));
				const RECT rect = {
					x,
					y,
					static_cast<int>(x + ((wind / max_wind) * 45.0f)),
					y + 10};
				FillRect(EngineHandle::get_handle().lock()->get_buffer_dc(), &rect, brush);
				DeleteObject(brush);
			}();

			// shooting angle
			// @todo: fix length reduction
			[this, hud_position]()
			{
				const int x = 90;
				const int y = hud_position.get_y() + 60;

				const auto default_angle = 
					(m_self.lock()->get_offset_top_forward_position() - m_self.lock()->get_center())
					.normalized()
					.unit_angle() - (Math::PI / 2);

				const auto offset_flip = default_angle;
				const auto move_angle = m_self.lock()->get_movement_pitch_radian();

				const auto white = Pen(Color(255, 255, 255, 0), 1);

				const UnitVector angle_vector = Math::Vector2::angle_vector(move_angle);
				const GlobalPosition ground_angle_s = Math::Vector2{x - 30, y} - (angle_vector * 30);
				const GlobalPosition ground_angle_e = Math::Vector2{x + 30, y} + (angle_vector * 30);

				EngineHandle::get_handle().lock()->get_buffer_gdi_handle().lock()->DrawLine(
					&white,
					ground_angle_s,
					ground_angle_e);

				const auto redpen = Pen(Color(255, 255, 0, 0), 1);

				const Math::Vector2 mid_point = {x, y};
				const Math::Vector2 end_point = Math::Vector2{0.0f, 55.0f}.rotate(move_angle + offset_flip);

				EngineHandle::get_handle().lock()->get_buffer_gdi_handle().lock()->DrawLine(
					&redpen,
					mid_point,
					mid_point + end_point);
			}();

			// items
			[this, hud_position]()
			{
				const int x = 253;
				const int y = hud_position.get_y() + 15;
				int interval = 0;

				for(const auto& [num, ptr] : m_self.lock()->get_available_items())
				{
					if(const auto item = ptr.lock())
					{
						const auto item_icon = item->get_icon_thumbnail().lock();
						const Math::Vector2 center = {x + interval, y};

						if(!item->is_used())
						{
							const auto green_light = SolidBrush(Color(0, 255, 155));

							EngineHandle::get_handle().lock()->get_buffer_gdi_handle().lock()->FillRectangle(
								&green_light,
								RectF
								{
									center,
									item_icon->get_hitbox()
								});
						}

						constexpr int slot_size = 30;
						constexpr int line_padding = 5;
						const auto size = item_icon->get_hitbox();
						item_icon->render(center, size);
						interval += slot_size + line_padding;
					}
				}
			}();
		}
		
		m_radar->render();
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

	std::weak_ptr<Round> BattleScene::get_round_status()
	{
		return m_round;
	}

	const Math::Vector2& BattleScene::get_map_size() const
	{
		return m_map_size;
	}

	std::vector<CharacterPointer> BattleScene::get_characters() const
	{
		std::vector<CharacterPointer> ret;

		for(const auto& [pid, ch] : m_characters)
		{
			ret.push_back(ch);
		}

		return ret;
	}

	std::vector<GroundPointer> BattleScene::get_grounds() const
	{
		return m_grounds;
	}

	bool BattleScene::predicate_OOB(const Math::Vector2& position)
	{
		return !movable(position);
	}

	bool BattleScene::movable(const Math::Vector2& position)
	{
		constexpr float epsilon = 500.0f;

		if(position.get_y() <= m_map_size.get_y() + epsilon)
		{
			return true;
		}

		return false;
	}

	void BattleScene::spawnpoints() const
	{
		int ground_x_min = INT_MAX;
		int ground_x_max = INT_MIN;

		for(const auto& ptr : m_grounds)
		{
			if(const auto ground = ptr.lock())
			{
				ground_x_min = std::min(ground_x_min, static_cast<int>(ground->get_top_left().get_x()));
				ground_x_max = std::max(ground_x_max, static_cast<int>(ground->get_top_right().get_x()));
			}
		}

		const int x_interval = ground_x_max / static_cast<int>(m_characters.size());
		int pivot = ground_x_min;
		int x_current = pivot + 150;

		// @todo: random spawnpoints
		for(const auto& [id, ch_ptr] : m_characters)
		{
			if(const auto character = ch_ptr.lock())
			{
				character->m_position = {x_current, -1000};

				for(const auto& gr_ptr : m_grounds)
				{
					if(const auto ground = gr_ptr.lock())
					{
						const auto delta = ground->safe_orthogonal_surface_global(character->get_center());

						if(delta == Math::vector_inf)
						{
							continue;
						}

						character->m_position += delta;
						character->m_position -= {0 , character->m_hitbox.get_y() / 2};
						break;
					}
				}
			}

			// @todo: multiple characters more than two
			pivot = (pivot == ground_x_min ? ground_x_max : ground_x_min);
			x_current = pivot - x_interval;
		}

	}

	Math::Vector2 BattleScene::evaluate_map_size() const
	{
		int top_left_x = INT_MAX;
		int top_left_y = INT_MAX;

		int bottom_right_x = INT_MIN;
		int bottom_right_y = INT_MIN;

		for(const auto& ptr : m_grounds)
		{
			if(const auto ground = ptr.lock())
			{
				top_left_x = std::min(top_left_x, static_cast<int>(ground->get_top_left().get_x()));
				top_left_y = std::min(top_left_y, static_cast<int>(ground->get_top_left().get_y()));

				bottom_right_x = std::max(
					bottom_right_x, 
					static_cast<int>(ground->get_bottom_right().get_x()));

				bottom_right_y = std::max(
					bottom_right_y, 
					static_cast<int>(ground->get_bottom_right().get_y()));
			}
		}

		const auto evaluated_size = Math::Vector2{bottom_right_x - top_left_x, bottom_right_y - top_left_y};

		return evaluated_size;
	}
}
