#ifndef RADAR_HPP
#define RADAR_HPP

#include "framework.h"
#include "ground.hpp"

namespace Fortress
{
	class Radar
	{
	public:
		Radar(const Math::Vector2& map_size) : m_center(map_size / 2), m_map_size(map_size)
		{
			initialize();
		}
		void initialize();
		void update();
		void render() const;

		HDC get_radar_hdc() const;
	private:
		Math::Vector2 m_center;
		Math::Vector2 m_map_size;
		BLENDFUNCTION m_bf;

		HDC m_radar_hdc;
		HBITMAP m_radar_bitmap;

		std::unique_ptr<Graphics> m_gdi_handle;
	};

	inline void Radar::initialize()
	{
		m_radar_hdc = CreateCompatibleDC(WinAPIHandles::get_main_dc());
		m_radar_bitmap = CreateCompatibleBitmap(WinAPIHandles::get_main_dc(), 1000, 1000);

		const auto previousBitmap = static_cast<HBITMAP>(SelectObject(m_radar_hdc, m_radar_bitmap));
		DeleteObject(previousBitmap);

		m_gdi_handle.reset(Graphics::FromHDC(m_radar_hdc));

		m_bf.AlphaFormat = 0;
		m_bf.BlendFlags = 0;
		m_bf.BlendOp = AC_SRC_OVER;
		m_bf.SourceConstantAlpha = 127;
	}

	inline void Radar::update()
	{
		m_gdi_handle->Clear(Color(255, 0, 0, 0));

		if(const auto scene = Scene::SceneManager::get_active_scene().lock())
		{
			const auto objects = scene->get_objects();
			const auto green = SolidBrush(Color(255, 0, 255, 0));

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

					const auto rect = RectF
					{
						position.get_x(),
						position.get_y(),
						20,
						20
					};

					// skip the dead character to be rendered.
					if(const auto ch = std::dynamic_pointer_cast<ObjectBase::character>(obj))
					{
						if(ch->get_state() == eCharacterState::Dead)
						{
							continue;
						}

						m_gdi_handle->FillEllipse(&green, rect);
					}
					else if(const auto gr = std::dynamic_pointer_cast<Object::Ground>(obj))
					{
						const auto bitmap = gr->get_mask_bitmap_copy();

						m_gdi_handle->DrawImage(
							bitmap.get(),
							PointF{
								position.get_x(),
								position.get_y()});
					}
				}
			}
		}
	}

	inline void Radar::render() const
	{
		GdiAlphaBlend(
			WinAPIHandles::get_buffer_dc(), 
			500,
			10,
			250,
			100,
			m_radar_hdc,
			0,
			0,
			1000,
			1000,
			m_bf);
	}

	inline HDC Radar::get_radar_hdc() const
	{
		return m_radar_hdc;
	}
}
#endif // RADAR_HPP
