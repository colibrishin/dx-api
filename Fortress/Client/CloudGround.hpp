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
		std::weak_ptr<ImageWrapper> m_cloud_image;
	};

	inline void CloudGround::initialize()
	{
		Ground::initialize();
		
		m_cloud_image = Resource::ResourceManager::load<ImageWrapper>(
			L"CloudGround", "./resources/misc/tiles/cloud-transparent.png");

		const auto initial_mask = Resource::ResourceManager::load<ImageWrapper>(
			L"CloudGround Mask", "./resources/misc/tiles/cloud-mask.bmp");

		const auto cloud = m_cloud_image.lock();
		const auto cloud_mask = initial_mask.lock();

		set_hitbox(cloud->get_hitbox());
		reset_hdc();

		cloud->copy_to(m_ground_hdc);
		cloud_mask->copy_to(m_mask_hdc);

		std::for_each(
			std::execution::par,
			m_destroyed_table.begin(),
			m_destroyed_table.end(),
			[&](const std::pair<GroundMapKey, GroundState>& pos)
			{
				const int y = pos.first.first;
				const int x = pos.first.second;
				const COLORREF pixel = get_pixel_threadsafe(x, y);

				if(pixel == 0)
				{
					unsafe_set_destroyed(x, y);
				}
		});

		Resource::ResourceManager::unload<ImageWrapper>(L"CloudGround Mask");
	}
}
#endif // CLOUD_HPP
