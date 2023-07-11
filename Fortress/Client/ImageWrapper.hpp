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
		ImageWrapper(const std::wstring& name, const std::filesystem::path& path);
		ImageWrapper& operator=(const ImageWrapper& other) = default;
		ImageWrapper& operator=(ImageWrapper&& other) = default;
		virtual ~ImageWrapper() override;

		virtual void render(const Math::Vector2& position, const Math::Vector2& facing);
		const Math::Vector2& get_hitbox() const;
		virtual void flip();

	protected:
		virtual bool load() override;
		Image* m_image;
		Graphics* m_gdi_handle;
		Math::Vector2 m_size;
		ImageAttributes m_chroma_key;
	};

	inline void ImageWrapper::flip()
	{
		m_image->RotateFlip(RotateNoneFlipX);
	}

	inline void ImageWrapper::render(const Math::Vector2& position, const Math::Vector2& facing = {})
	{
		if(m_image)
		{
			m_gdi_handle->DrawImage(
				m_image,
				RectF{
				position.get_x(),
				position.get_y(),
				m_size.get_x(),
				m_size.get_y()},
				0,
				0,
				-m_size.get_x(),
				-m_size.get_y(),
				UnitPixel,
				&m_chroma_key);
		}
	}

	inline const Math::Vector2& ImageWrapper::get_hitbox() const
	{
		return m_size;
	}

	inline ImageWrapper::ImageWrapper(const std::wstring& name, const std::filesystem::path& path) :
		Resource(name, path),
		m_image(nullptr),
		m_gdi_handle(nullptr),
		m_size{}
	{
	}

	inline ImageWrapper::~ImageWrapper()
	{
		if(m_image)
		{
			delete m_image;
		}

		if(m_gdi_handle)
		{
			delete m_gdi_handle;
		}

		if(m_image)
		{
			DeleteObject(m_image);
		}

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

		const auto magenta = Color(255,0,255);
		m_chroma_key.SetColorKey(magenta, magenta, ColorAdjustTypeBitmap);  

		return true;
	}
}
#endif // ImageWrapper_HPP
