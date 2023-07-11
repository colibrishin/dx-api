// yes, include guard is added twice.
#pragma once
#ifndef GIFWRAPPER_HPP
#define GIFWRAPPER_HPP

#include "ImageWrapper.hpp"

#include <map>
#include <objidl.h>
#include <gdiplus.h>
using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")

namespace Fortress
{
	class GifWrapper : public ImageWrapper
	{
	public:
		GifWrapper(const std::wstring& name, const std::filesystem::path& path);
		GifWrapper& operator=(const GifWrapper& other) = default;
		GifWrapper& operator=(GifWrapper&& other) = default;
		virtual ~GifWrapper() override;

		void render(const Math::Vector2& position, const Math::Vector2& facing) override;
		virtual bool load() override;
		virtual void initialize();
		void play();
		void OnTimer();
		virtual void flip() override;

		inline static std::map<UINT, GifWrapper*> registered_gifs = {};
	private:
		UINT m_timer_id;
		UINT m_dimension_count;
		UINT m_frame_count;
		UINT m_total_buffer;
		UINT m_current_frame;
		WCHAR m_str_guid[39];

		GUID* m_pDimensionsIds;
		PropertyItem* m_property_item;

		bool m_bFlipped;
		inline static UINT used_timer_id = 0;
	};

	inline void GifWrapper::render(const Math::Vector2& position, const Math::Vector2& facing = {})
	{
		if(m_image)
		{
			m_gdi_handle->DrawImage(
				m_image,
				Rect{
				static_cast<int>(position.get_x()),
				static_cast<int>(position.get_y()),
				static_cast<int>(m_size.get_x()),
				static_cast<int>(m_size.get_y())},
				0,
				0,
				static_cast<int>(m_size.get_x()),
				static_cast<int>(m_size.get_y()),
				UnitPixel,
				&m_chroma_key);
		}
	}

	inline bool GifWrapper::load()
	{
		ImageWrapper::load();

		m_dimension_count = m_image->GetFrameDimensionsCount();

		m_pDimensionsIds = new GUID[m_dimension_count];
		m_image->GetFrameDimensionsList(m_pDimensionsIds, m_dimension_count);

		StringFromGUID2(m_pDimensionsIds[0], m_str_guid, 39);
		m_frame_count = m_image->GetFrameCount(&m_pDimensionsIds[0]);

		m_total_buffer = m_image->GetPropertyItemSize(PropertyTagFrameDelay);
		m_property_item = new PropertyItem[m_total_buffer];

		m_image->GetPropertyItem(PropertyTagFrameDelay, m_total_buffer, m_property_item);

		return true;
	}

	inline void GifWrapper::initialize()
	{
		registered_gifs[m_timer_id] = this;
	}

	inline void GifWrapper::play()
	{
		KillTimer(WinAPIHandles::get_hwnd(), m_timer_id);

		m_current_frame = 0;
		const GUID guid = FrameDimensionTime;

		m_image->SelectActiveFrame(&guid, m_current_frame);
		if(m_bFlipped)
		{
			m_image->RotateFlip(RotateNoneFlipX);
		}

		SetTimer(WinAPIHandles::get_hwnd(), m_timer_id, ((UINT*)m_property_item[0].value)[m_current_frame] * 10, nullptr);

		++m_current_frame;
	}

	inline void GifWrapper::OnTimer()
	{
		KillTimer(WinAPIHandles::get_hwnd(), m_timer_id);

		const GUID guid = FrameDimensionTime;
		m_image->SelectActiveFrame(&guid, m_current_frame);


		SetTimer(WinAPIHandles::get_hwnd(), 1, static_cast<UINT*>(m_property_item[0].value)[m_current_frame] * 10, nullptr);

		m_current_frame = ++(m_current_frame) % m_frame_count;
	}

	inline void GifWrapper::flip()
	{
		// @todo: is there anyway to not break the gif property?
	}

	inline GifWrapper::GifWrapper(const std::wstring& name, const std::filesystem::path& path) :
		ImageWrapper(name, path), m_dimension_count(0), m_frame_count(0), m_total_buffer(0), m_current_frame(0),
		m_str_guid{}, m_pDimensionsIds(nullptr), m_property_item(nullptr), m_bFlipped(false), m_timer_id(used_timer_id)
	{
		used_timer_id++;
		GifWrapper::initialize();
	}

	inline GifWrapper::~GifWrapper()
	{
		ImageWrapper::~ImageWrapper();

		registered_gifs.erase(m_timer_id);

		if(m_pDimensionsIds)
		{
			delete[] m_pDimensionsIds;
		}

		if(m_property_item)
		{
			free(m_property_item);
		}
	}
}
#endif // GIFWRAPPER_HPP
