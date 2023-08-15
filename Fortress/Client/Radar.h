#ifndef RADAR_HPP
#define RADAR_HPP

#include "ground.hpp"

namespace Fortress
{
	constexpr float radar_padding = 100.0f;

	class Radar
	{
	public:
		Radar(const Math::Vector2& map_size) : m_center(map_size / 2), m_map_size(map_size + radar_padding)
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
}
#endif // RADAR_HPP
