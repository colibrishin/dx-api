#ifndef RADAR_HPP
#define RADAR_HPP

#include "resource.h"
#include "framework.h"
#include "ground.hpp"

#include "sceneManager.hpp"
#include "winapihandles.hpp"

#pragma comment(lib, "msimg32.lib")

namespace Fortress
{
	class Radar
	{
	public:
		Radar() = default;
		void initialize();
		void update() const;
		void render() const;

		HDC get_radar_hdc() const;
	private:
		Math::Vector2 m_center;
		BLENDFUNCTION m_bf;
		HDC m_radar_hdc;
		HBITMAP m_radar_bitmap;
	};

	inline void Radar::initialize()
	{
		m_radar_hdc = CreateCompatibleDC(WinAPIHandles::get_main_dc());
		// @todo: total size of battle ground should be given.
		m_radar_bitmap = CreateCompatibleBitmap(m_radar_hdc, 1000, 1000);
		const auto previousBitmap = static_cast<HBITMAP>(SelectObject(m_radar_hdc, m_radar_bitmap));
		DeleteObject(previousBitmap);

		m_center = {500, 500};

		m_bf.AlphaFormat = 0;
		m_bf.BlendFlags = 0;
		m_bf.BlendOp = AC_SRC_OVER;
		m_bf.SourceConstantAlpha = 127;
	}

	inline void Radar::update() const
	{
		constexpr auto radar_size = RECT{0, 0, 1000, 1000};
		const auto black = CreateSolidBrush(BLACK_BRUSH);
		FillRect(m_radar_hdc, &radar_size, black);
		DeleteObject(black);

		if(const auto scene = Scene::SceneManager::get_active_scene().lock())
		{
			const auto objects = scene->get_objects();
			const auto green = CreateSolidBrush(RGB(0, 255, 0));
			const auto white = static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));

			for(const auto& ptr : objects)
			{
				if(const auto obj = ptr.lock())
				{
					// we don't need the deactivated object to be rendered.
					if (!obj->is_active())
					{
						continue;
					}

					const Math::Vector2 position = obj->get_top_left() + m_center;
					const Math::Vector2 size = obj->m_hitbox;

					// skip the dead character to be rendered.
					if(const auto ch = std::dynamic_pointer_cast<ObjectBase::character>(obj))
					{
						if(ch->get_state() == eCharacterState::Dead)
						{
							continue;
						}

						// note that we are using windows api rect here.
						// third and forth value should be the position of the endpoint x and y
						const auto obj_rect = RECT{
							static_cast<int>(position.get_x()),
							static_cast<int>(position.get_y()),
							static_cast<int>(position.get_x() + 10),
							static_cast<int>(position.get_y() + 10)};
						
						FillRect(m_radar_hdc, &obj_rect, green);
					}
					else if(const auto gr = std::dynamic_pointer_cast<Object::Ground>(obj))
					{
						BitBlt(
							m_radar_hdc,
							position.get_x(),
							position.get_y(),
							size.get_x(),
							size.get_y(),
							gr->get_ground_hdc(),
							0,
							0,
							SRCCOPY);
					}
				}
			}

			DeleteObject(green);
			DeleteObject(white);
		}
	}

	inline void Radar::render() const
	{
		AlphaBlend(WinAPIHandles::get_buffer_dc(), 400, 10, 300, 100, m_radar_hdc, 0, 0, 1000, 1000, m_bf);
	}

	inline HDC Radar::get_radar_hdc() const
	{
		return m_radar_hdc;
	}
}
#endif // RADAR_HPP
