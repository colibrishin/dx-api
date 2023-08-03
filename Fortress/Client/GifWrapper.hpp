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
	constexpr int gif_timer_id = 8000;

	class GifWrapper : public ImageWrapper
	{
	public:
		GifWrapper(const std::wstring& name, const std::filesystem::path& path);
		GifWrapper& operator=(const GifWrapper& other) = default;
		GifWrapper& operator=(GifWrapper&& other) = default;
		GifWrapper(const GifWrapper& other) = default;
		GifWrapper(GifWrapper&& other) = default;
		virtual ~GifWrapper() override;

		virtual bool load() override;
		virtual void initialize();
		static void cleanup();

		void play(const std::function<void()>& on_end = {});
		void stop() const;
		void OnTimer();
		virtual void flip() override;
		virtual void rotate(const float angle);
		void reset_transfrom();

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

		inline static UINT used_timer_id = gif_timer_id;
	};

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
	        [](unsigned int n) {return n; }
	    );

		return true;
	}

	inline void GifWrapper::initialize()
	{
		registered_gifs[m_timer_id] = this;
	}

	inline void GifWrapper::cleanup()
	{
		for(const auto& f : registered_gifs)
		{
			f.second->stop();
			f.second->ImageWrapper::cleanup();
		}

		registered_gifs.clear();
	}

	inline void GifWrapper::play(const std::function<void()>& on_end)
	{
		if(on_end != nullptr)
		{
			m_reserved_function = on_end;
		}

		m_current_frame = 0;
		const GUID guid = FrameDimensionTime;

		m_image->SelectActiveFrame(&guid, m_current_frame);

		KillTimer(WinAPIHandles::get_hwnd(), m_timer_id);
		m_timer_id = SetTimer(WinAPIHandles::get_hwnd(), m_timer_id, m_frame_delays[0], nullptr);

		++m_current_frame;
	}

	inline void GifWrapper::stop() const
	{
		KillTimer(WinAPIHandles::get_hwnd(), m_timer_id);
	}

	inline void GifWrapper::OnTimer()
	{
		KillTimer(WinAPIHandles::get_hwnd(), m_timer_id);

		const GUID guid = FrameDimensionTime;
		m_image->SelectActiveFrame(&guid, m_current_frame);

		m_timer_id = SetTimer(WinAPIHandles::get_hwnd(), m_timer_id, m_frame_delays[m_current_frame] * 10, nullptr);

		if(m_current_frame == m_frame_count - 1)
		{
			if(m_reserved_function != nullptr) 
			{
				m_reserved_function();
				m_reserved_function = nullptr;
			}
		}

		m_current_frame = ++(m_current_frame) % m_frame_count;
	}

	inline void GifWrapper::flip()
	{
		ImageWrapper::flip();
		// @todo: is there anyway to not break the gif property?
	}

	inline void GifWrapper::rotate(const float angle)
	{
		m_gdi_handle->RotateTransform(angle);
	}

	inline void GifWrapper::reset_transfrom()
	{
		m_gdi_handle->ResetTransform();
	}

	inline GifWrapper::GifWrapper(
		const std::wstring& name,
		const std::filesystem::path& path) :
		ImageWrapper(name, path),
		m_dimension_count(0),
		m_frame_count(0),
		m_total_buffer(0),
		m_current_frame(0),
		m_str_guid{},
		m_timer_id(used_timer_id++)
	{
		GifWrapper::initialize();
	}

	inline GifWrapper::~GifWrapper()
	{
		stop();
		if(registered_gifs.find(m_timer_id) != registered_gifs.end())
		{
			registered_gifs.erase(m_timer_id);
		}
		ImageWrapper::~ImageWrapper();
	}
}
#endif // GIFWRAPPER_HPP
