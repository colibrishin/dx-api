// yes, include guard is added twice.
#pragma once
#ifndef ImageWrapper_HPP
#define ImageWrapper_HPP
#include "resource.hpp"
#include "vector2.hpp"
#include "winapihandles.hpp"

#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")

namespace Fortress
{
	class ImageWrapper : public Abstract::Resource
	{
	public:
		ImageWrapper(const std::wstring& name, const std::filesystem::path& path, const Math::Vector2& offset);
		ImageWrapper& operator=(const ImageWrapper& other) = default;
		ImageWrapper& operator=(ImageWrapper&& other) = default;
		virtual ~ImageWrapper() override;

		virtual void render(const Math::Vector2& position, const Math::Vector2& hitbox, const Math::Vector2& scaling = {1.0f, 1.0f});
		const Math::Vector2& get_hitbox() const;
		virtual void flip();
		void set_offset(const Math::Vector2& offset);

	protected:
		virtual bool load() override;
		Image* m_image;
		Graphics* m_gdi_handle;
		Math::Vector2 m_size;
		Math::Vector2 m_offset;
	};

	inline void ImageWrapper::flip()
	{
		m_image->RotateFlip(RotateNoneFlipX);
	}

	inline void ImageWrapper::set_offset(const Math::Vector2& offset)
	{
		m_offset = offset;
	}

	inline void ImageWrapper::render(
		const Math::Vector2& position,
		const Math::Vector2& hitbox,
		const Math::Vector2& scaling)
	{
		if(m_image)
		{
			const Math::Vector2 scaled_m_size = m_size * scaling;
			const Math::Vector2 hitbox_diff = hitbox - scaled_m_size;

			m_gdi_handle->DrawImage(
				m_image,
				RectF{
				position.get_x() + hitbox_diff.get_x() + m_offset.get_x(),
				position.get_y() + hitbox_diff.get_y() + m_offset.get_y(),
				scaled_m_size.get_x(),
				scaled_m_size.get_y()},
				0,
				0,
				m_size.get_x(),
				m_size.get_y(),
				UnitPixel,
				nullptr);
		}
	}

	inline const Math::Vector2& ImageWrapper::get_hitbox() const
	{
		return m_size;
	}

	inline ImageWrapper::ImageWrapper(const std::wstring& name, const std::filesystem::path& path, const Math::Vector2& offset) :
		Resource(name, path),
		m_image(nullptr),
		m_gdi_handle(nullptr),
		m_size{},
		m_offset(offset)
	{
	}

	inline ImageWrapper::~ImageWrapper()
	{
		Resource::~Resource();
	}

	inline bool ImageWrapper::load()
	{
		m_image = new Image(get_path().native().c_str());

		if(!m_image)
		{
			return false;
		}

		m_size = {static_cast<float>(m_image->GetWidth()), static_cast<float>(m_image->GetHeight())};
		m_gdi_handle = new Graphics{WinAPIHandles::get_buffer_dc()};

		constexpr float range = 3.0f;
		const auto magenta_high = Color(255, 255 / range,255);
		const auto magenta_low = Color(255 / range, 0,255 / range);

		return true;
	}
}
#endif // ImageWrapper_HPP
