// yes, include guard is added twice.
#pragma once
#ifndef ImageWrapper_HPP
#define ImageWrapper_HPP
#include "resource.hpp"
#include "vector2.hpp"

#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>

#include "EngineHandle.h"

using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")

namespace Fortress
{
	class ImageWrapper : public Abstract::Resource
	{
	public:
		ImageWrapper(const std::wstring& name, const std::filesystem::path& path);
		ImageWrapper& operator=(const ImageWrapper& other) = default;
		ImageWrapper& operator=(ImageWrapper&& other) = default;
		virtual ~ImageWrapper() override = default;

		void cleanup();
		virtual void render(
			const Math::Vector2& center_position, 
			const Math::Vector2& hitbox, 
			const Math::Vector2& scaling = {1.0f, 1.0f},
			const float rotate_degree = 0.0f);
		const Math::Vector2& get_hitbox() const;
		virtual void flip();
		void set_offset(const Math::Vector2& offset);
		void set_rotation_offset(const Math::Vector2& offset);
		void copy_to(HDC) const;
		void tile_copy_to(const Math::Vector2& size, HDC) const;

	protected:
		virtual bool load() override;
		std::unique_ptr<Image> m_image;
		std::unique_ptr<Graphics> m_gdi_handle;
		Math::Vector2 m_size;
		Math::Vector2 m_offset;
		Math::Vector2 m_rotation_offset;
	};

	inline void ImageWrapper::flip()
	{
		m_image->RotateFlip(RotateNoneFlipX);
	}

	inline void ImageWrapper::set_offset(const Math::Vector2& offset)
	{
		m_offset = offset;
	}

	/**
	 * \brief A offset for rotation mid point
	 * \param offset a offset to manipulate mid point
	 */
	inline void ImageWrapper::set_rotation_offset(const Math::Vector2& offset)
	{
		m_rotation_offset = offset;
	}

	inline void ImageWrapper::copy_to(HDC target) const
	{
		Graphics temp(target);

		temp.DrawImage(m_image.get(), 0.0f, 0.0f, m_size.get_x(), m_size.get_y());
	}

	inline void ImageWrapper::tile_copy_to(const Math::Vector2& size, HDC target) const
	{
		Graphics temp(target);

		float start_x = 0;
		float start_y = 0;

		for(; 
			start_y < size.get_y(); 
			start_y += m_size.get_y())
		{
			for(; 
				start_x < size.get_x(); 
				start_x += m_size.get_x())
			{
				temp.DrawImage(m_image.get(), start_x, start_y);
			}
		}

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
			const Math::Vector2 image_mid = (top_left + scaled_m_size / 2) + m_rotation_offset;

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

	inline ImageWrapper::ImageWrapper(
		const std::wstring& name, const std::filesystem::path& path) :
		Resource(name, path),
		m_image(nullptr),
		m_gdi_handle(nullptr),
		m_size{},
		m_offset{},
		m_rotation_offset{}
	{
		ImageWrapper::load();
	}

	inline bool ImageWrapper::load()
	{
		m_image = std::make_unique<Image>(get_path().native().c_str());

		if(!m_image)
		{
			return false;
		}

		m_size = {static_cast<float>(m_image->GetWidth()), static_cast<float>(m_image->GetHeight())};
		m_gdi_handle = std::make_unique<Graphics>(EngineHandle::get_handle().lock()->get_buffer_dc());
		return true;
	}
}
#endif // ImageWrapper_HPP
