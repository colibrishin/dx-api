#ifndef CLOUD_HPP
#define CLOUD_HPP
#include "ground.hpp"

namespace Fortress::Object
{
	class CloudGround final : public Ground
	{
	public:
		CloudGround(
			const std::wstring& name, 
			const Math::Vector2& position) :
			Ground
			{
				name, 
				position, 
				Resource::ResourceManager::load<ImageWrapper>(
					L"CloudGround", 
					"./resources/misc/tiles/cloud-transparent.png").lock()->get_hitbox()
			}
		{
			CloudGround::initialize();
		}
		CloudGround& operator=(const CloudGround& other) = default;
		CloudGround& operator=(CloudGround&& other) = default;

		void initialize() override;
	};

	inline void CloudGround::initialize()
	{
		Ground::initialize();
		
		const auto cloud_image = 
			Resource::ResourceManager::find<ImageWrapper>(L"CloudGround");
		const auto initial_mask = 
			Resource::ResourceManager::load<ImageWrapper>(
			L"CloudGround Mask", "./resources/misc/tiles/cloud-mask.bmp");

		const auto cloud = cloud_image.lock();
		const auto cloud_mask = initial_mask.lock();

		set_hitbox(cloud->get_hitbox());

		cloud->copy_to(m_ground_hdc);
		cloud_mask->copy_to(m_mask_hdc);

		const auto mask_gdi = Bitmap::FromHBITMAP(m_mask_bitmap, nullptr);
		BitmapData bitmap_info{};
		
		const UINT width = mask_gdi->GetWidth();
		const UINT height = mask_gdi->GetHeight();

		const Rect mask_size = Rect
		{
			0,
			0,
			static_cast<int>(width),
			static_cast<int>(height)
		};

		// assuming we are using hdc only.
		mask_gdi->LockBits(
			&mask_size,
			ImageLockModeRead,
			PixelFormat32bppARGB,
			&bitmap_info);

		// used type unsigned int (4 bytes, 1b-A 1b-R 1b-G 1b-B)
		auto* const direct_access_ptr = static_cast<unsigned int*>(bitmap_info.Scan0);
		const UINT stride = std::abs(bitmap_info.Stride);

		// probably due to non 4-byte reads happened.
		assert(stride / 4 == width);
		const UINT array_size = width * height;
		// A 1111 1111 R 0000 0000 G 0000 0000 B 0000 0000
		constexpr unsigned int alpha_black = UINT_MAX << 24;

		std::for_each(
			std::execution::par,
			direct_access_ptr,
			direct_access_ptr + array_size,
			[&](unsigned int& pixel)
			{
				const size_t index = std::distance(direct_access_ptr, &pixel);
				const size_t y = index / width;
				const size_t x = index % width;

				if(pixel == alpha_black)
				{
					unsafe_set_destroyed(static_cast<int>(x), static_cast<int>(y));
				}
		});

		mask_gdi->UnlockBits(&bitmap_info);
		DeleteObject(mask_gdi);

		force_update_mask();

		Resource::ResourceManager::unload<ImageWrapper>(L"CloudGround Mask");
		Resource::ResourceManager::unload<ImageWrapper>(L"CloudGround");
	}
}
#endif // CLOUD_HPP
