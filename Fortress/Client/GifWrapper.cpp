#include "GifWrapper.h"

#include "GifTimer.hpp"

namespace Fortress
{
	bool GifWrapper::load()
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
	        [](unsigned int n) {return n * 10; }
	    );

		return true;
	}

	void GifWrapper::initialize()
	{
		m_timer = ObjectBase::TimerManager::create<GifTimer>(&GifWrapper::OnTimer, this);
		load();
	}

	void GifWrapper::play(const std::function<void()>& on_end)
	{
		if(on_end != nullptr)
		{
			m_reserved_function = on_end;
		}

		m_current_frame = 0;
		const GUID guid = FrameDimensionTime;

		m_image->SelectActiveFrame(&guid, m_current_frame);

		m_timer.lock()->set_duration(m_frame_delays[m_current_frame]);
		m_timer.lock()->toggle();

		++m_current_frame;
	}

	void GifWrapper::stop() const
	{
		m_timer.lock()->stop();
	}

	void GifWrapper::OnTimer()
	{
		const GUID guid = FrameDimensionTime;
		m_image->SelectActiveFrame(&guid, m_current_frame);

		m_timer.lock()->set_duration(m_frame_delays[m_current_frame]);
		m_timer.lock()->toggle();

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

	void GifWrapper::flip()
	{
		ImageWrapper::flip();
		// @todo: is there anyway to not break the gif property?
	}

	void GifWrapper::rotate(const float angle)
	{
		m_gdi_handle->RotateTransform(angle);
	}

	void GifWrapper::reset_transform()
	{
		m_gdi_handle->ResetTransform();
	}

	unsigned GifWrapper::get_total_play_time() const
	{
		return std::accumulate(m_frame_delays.begin(), m_frame_delays.end(), static_cast<unsigned int>(0));
	}

	GifWrapper::GifWrapper(
		const std::wstring& name,
		const std::filesystem::path& path) :
		ImageWrapper(name, path),
		m_dimension_count(0),
		m_frame_count(0),
		m_total_buffer(0),
		m_current_frame(0),
		m_str_guid{}
	{
		initialize();
	}

	GifWrapper::~GifWrapper()
	{
		stop();
		ObjectBase::TimerManager::remove(std::dynamic_pointer_cast<Timer>(m_timer.lock()));
	}
}
