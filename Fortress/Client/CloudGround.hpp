#ifndef CLOUD_HPP
#define CLOUD_HPP
#include "ground.hpp"

namespace Fortress::Object
{
	class CloudGround final : public Ground
	{
	public:
		CloudGround(const std::wstring& name, 
			const Math::Vector2& position) :
			Ground(name, position, {})
		{
			CloudGround::initialize();
		}
		CloudGround& operator=(const CloudGround& other) = default;
		CloudGround& operator=(CloudGround&& other) = default;

		void initialize() override;
	private:
		// 1524 x 365
		std::weak_ptr<ImageWrapper> m_cloud_image;
	};

	inline void CloudGround::initialize()
	{
		Ground::initialize();
		
		m_cloud_image = Resource::ResourceManager::load<ImageWrapper>(
			L"CloudGround", "./resources/misc/tiles/cloud-magenta.png");

		const auto cloud = m_cloud_image.lock();

		set_hitbox(cloud->get_hitbox());
		reset_hdc();

		cloud->copy_to(m_ground_hdc);

		const auto size = cloud->get_hitbox();
		constexpr auto magenta = RGB(255, 0, 255);

		// @todo: performance bottleneck, optimization is needed.
		for(int y = 0; y < static_cast<int>(size.get_y()); ++y)
		{
			for(int x = 0; x < static_cast<int>(size.get_x()); ++x)
			{
				if(const COLORREF pixel = GetPixel(m_ground_hdc, x, y);
					pixel == magenta)
				{
					unsafe_set_destroyed(x, y);
					unsafe_set_destroyed_visual(x, y);
				}
			}
		}
	}
}
#endif // CLOUD_HPP
