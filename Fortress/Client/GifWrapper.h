#ifndef GIFWRAPPER_HPP
#define GIFWRAPPER_HPP

#include <functional>
#include <map>

#include "ImageWrapper.hpp"
#include <objidl.h>
#include <gdiplus.h>
#include <numeric>

using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")

#include "TimerManager.hpp"

namespace Fortress
{
	class GifTimer;

	class GifWrapper final : public ImageWrapper
	{
	public:
		GifWrapper(const std::wstring& name, const std::filesystem::path& path);
		GifWrapper& operator=(const GifWrapper& other) = default;
		GifWrapper& operator=(GifWrapper&& other) = default;
		GifWrapper(const GifWrapper& other) = default;
		GifWrapper(GifWrapper&& other) = default;
		~GifWrapper() override;

		bool load() override;
		void initialize();

		void play(const std::function<void()>& on_end = {});
		void stop() const;
		void flip() override;
		void rotate(const float angle);
		void reset_transform();
		unsigned int get_total_play_time() const;
		
	private:
		void OnTimer();

		std::weak_ptr<GifTimer> m_timer;
		UINT m_dimension_count;
		UINT m_frame_count;
		UINT m_total_buffer;
		UINT m_current_frame;
		WCHAR m_str_guid[39];

		std::vector<unsigned int> m_frame_delays;

		std::function<void()> m_reserved_function;
	};
}
#endif // GIFWRAPPER_HPP
