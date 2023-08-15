#include "Radar.h"
#include "BattleScene.h"

namespace Fortress
{
	void Radar::initialize()
	{
		m_radar_hdc = CreateCompatibleDC(
			WinAPIHandles::get_main_dc());
		m_radar_bitmap = CreateCompatibleBitmap(
			WinAPIHandles::get_main_dc(), m_map_size.get_x(), m_map_size.get_y());

		const auto previousBitmap = static_cast<HBITMAP>(SelectObject(m_radar_hdc, m_radar_bitmap));
		DeleteObject(previousBitmap);

		m_gdi_handle.reset(Graphics::FromHDC(m_radar_hdc));

		m_bf.AlphaFormat = 0;
		m_bf.BlendFlags = 0;
		m_bf.BlendOp = AC_SRC_OVER;
		m_bf.SourceConstantAlpha = 127;
	}

	void Radar::update()
	{
		m_gdi_handle->Clear(Color(255, 0, 0, 0));

		if(const auto scene = Scene::SceneManager::get_active_map().lock())
		{
			const auto green = SolidBrush(Color(255, 0, 255, 0));

			for(const auto& ptr : scene->get_grounds())
			{
				if(const auto obj = ptr.lock())
				{
					// we don't need the deactivated object to be rendered.
					if (!obj->is_active())
					{
						continue;
					}

					const Math::Vector2 position = obj->get_top_left() + m_center + (radar_padding / 2);

					if(const auto gr = std::dynamic_pointer_cast<Object::Ground>(obj))
					{
						const auto bitmap = gr->get_mask_bitmap_copy();

						m_gdi_handle->DrawImage(
							bitmap.get(),
							PointF
							{
								position.get_x(),
								position.get_y()
							});
					}
				}
			}

			for(const auto& ptr : scene->get_characters())
			{
				if(const auto obj = ptr.lock())
				{
					// we don't need the deactivated object to be rendered.
					if (!obj->is_active())
					{
						continue;
					}

					const Math::Vector2 position = obj->get_center() + m_center + (radar_padding / 2);

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

						m_gdi_handle->FillRectangle(&green, rect);
					}
				}
			}
		}
	}

	void Radar::render() const
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
			m_map_size.get_x(),
			m_map_size.get_y(),
			m_bf);
	}

	HDC Radar::get_radar_hdc() const
	{
		return m_radar_hdc;
	}
}
