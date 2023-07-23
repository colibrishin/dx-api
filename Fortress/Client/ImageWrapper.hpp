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

#include "debug.hpp"
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

		void cleanup();
		virtual void render(
			const Math::Vector2& center_position, 
			const Math::Vector2& hitbox, 
			const Math::Vector2& scaling = {1.0f, 1.0f},
			const float rotate_degree = 0.0f);
		const Math::Vector2& get_hitbox() const;
		virtual void flip();
		void set_offset(const Math::Vector2& offset);

	protected:
		virtual bool load() override;
		std::shared_ptr<Image> m_image;
		std::shared_ptr<Graphics> m_gdi_handle;
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
		const Math::Vector2& center_position,
		const Math::Vector2& hitbox,
		const Math::Vector2& scaling,
		const float rotate_degree)
	{
		if(m_image)
		{
			const Math::Vector2 scaled_m_size = m_size * scaling;
			const Math::Vector2 hitbox_diff = hitbox - scaled_m_size;

			const Math::Vector2 top_left = center_position + hitbox_diff + m_offset;
			const Math::Vector2 image_mid = top_left + scaled_m_size / 2;
			Debug::draw_dot(top_left);
			Debug::draw_dot(image_mid);

			if(rotate_degree != 0.0f)
			{
				m_gdi_handle->TranslateTransform(
					image_mid.get_x(), image_mid.get_y());
				m_gdi_handle->RotateTransform(rotate_degree);
				m_gdi_handle->TranslateTransform(
					-image_mid.get_x(), -image_mid.get_y());
			}

			m_gdi_handle->DrawImage(
				m_image.get(),
				RectF{
				top_left.get_x(),
				top_left.get_y(),
				scaled_m_size.get_x(),
				scaled_m_size.get_y()},
				0,
				0,
				m_size.get_x(),
				m_size.get_y(),
				UnitPixel,
				nullptr);

			if(rotate_degree != 0.0f)
			{
				m_gdi_handle->TranslateTransform(
					image_mid.get_x(), image_mid.get_y());
				m_gdi_handle->RotateTransform(-rotate_degree);
				m_gdi_handle->TranslateTransform(
					-image_mid.get_x(), -image_mid.get_y());
			}
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
		ImageWrapper::cleanup();
		Resource::~Resource();
	}

	inline void ImageWrapper::cleanup()
	{
		m_image.reset();
		m_gdi_handle.reset();
	}

	inline bool ImageWrapper::load()
	{
		m_image = std::make_shared<Image>(get_path().native().c_str());

		if(!m_image)
		{
			return false;
		}

		m_size = {static_cast<float>(m_image->GetWidth()), static_cast<float>(m_image->GetHeight())};
		m_gdi_handle = std::make_shared<Graphics>(WinAPIHandles::get_buffer_dc());

		return true;
	}
}
#endif // ImageWrapper_HPP
