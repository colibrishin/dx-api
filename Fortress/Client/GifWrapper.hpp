// yes, include guard is added twice.
#pragma once
#ifndef GIFWRAPPER_HPP
#define GIFWRAPPER_HPP

#include <functional>
#include <map>

#include "ImageWrapper.hpp"
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
		void play(const std::function<void()>& on_end);
		void OnTimer();
		virtual void flip() override;

		inline static std::map<UINT_PTR, GifWrapper*> registered_gifs = {};
	private:
		UINT_PTR m_timer_id;
		UINT m_dimension_count;
		UINT m_frame_count;
		UINT m_total_buffer;
		UINT m_current_frame;
		WCHAR m_str_guid[39];

		std::vector<unsigned int> m_frame_delays;

		std::function<void()> m_reserved_function;

		inline static UINT used_timer_id = 8000;
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

		const std::unique_ptr<GUID[]> m_pDimensionsIds (new GUID[m_dimension_count]);
		m_image->GetFrameDimensionsList(m_pDimensionsIds.get(), m_dimension_count);

		StringFromGUID2(m_pDimensionsIds[0], m_str_guid, 39);
		m_frame_count = m_image->GetFrameCount(&m_pDimensionsIds[0]);

		m_total_buffer = m_image->GetPropertyItemSize(PropertyTagFrameDelay);
		const std::unique_ptr<PropertyItem[]> property(new PropertyItem[m_total_buffer]);

		m_image->GetPropertyItem(PropertyTagFrameDelay, m_total_buffer, property.get());

		unsigned int* frame_delay_array = (unsigned int*)property.get()[0].value;

	    // copy the delay values into an std::vector while converting to milliseconds.
		m_frame_delays.assign(m_frame_count, 0);
	    std::transform(frame_delay_array, frame_delay_array + m_frame_count, m_frame_delays.begin(),
	        [](unsigned int n) {return n * 2.5; }
	    );

		return true;
	}

	inline void GifWrapper::initialize()
	{
	}

	inline void GifWrapper::play(const std::function<void()>& on_end = nullptr)
	{
		if(on_end != nullptr)
		{
			m_reserved_function = on_end;
		}

		m_current_frame = 0;
		const GUID guid = FrameDimensionTime;

		m_image->SelectActiveFrame(&guid, m_current_frame);

		m_timer_id = SetTimer(WinAPIHandles::get_hwnd(), m_timer_id, m_frame_delays[0], nullptr);
		registered_gifs[m_timer_id] = this;

		++m_current_frame;
	}

	inline void GifWrapper::OnTimer()
	{
		KillTimer(WinAPIHandles::get_hwnd(), m_timer_id);

		const GUID guid = FrameDimensionTime;
		m_image->SelectActiveFrame(&guid, m_current_frame);

		m_timer_id = SetTimer(WinAPIHandles::get_hwnd(), m_timer_id, m_frame_delays[m_current_frame] * 10, nullptr);
		registered_gifs[m_timer_id] = this;

		if(m_current_frame == m_frame_count - 1)
		{
			if(m_reserved_function != nullptr) 
			{
				m_reserved_function();
			}
		}

		m_current_frame = ++(m_current_frame) % m_frame_count;
	}

	inline void GifWrapper::flip()
	{
		ImageWrapper::flip();
		// @todo: is there anyway to not break the gif property?
	}

	inline GifWrapper::GifWrapper(const std::wstring& name, const std::filesystem::path& path) :
		ImageWrapper(name, path), m_dimension_count(0), m_frame_count(0), m_total_buffer(0), m_current_frame(0),
		m_str_guid{}, m_timer_id(used_timer_id++)
	{
		GifWrapper::initialize();
	}

	inline GifWrapper::~GifWrapper()
	{
		ImageWrapper::~ImageWrapper();

		registered_gifs.erase(m_timer_id);
	}
}
#endif // GIFWRAPPER_HPP
