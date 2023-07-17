#pragma once
#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <type_traits>
#include "GifWrapper.hpp"
#include "ImageWrapper.hpp"
#include "resourceManager.hpp"

namespace Fortress
{
	template <typename T = ImageWrapper>
	class Texture : public Abstract::entity
	{
	public:
		Texture() = delete;
		Texture& operator=(const Texture& other) = default;
		Texture& operator=(Texture&& other) = default;
		Texture(const Texture& other) = default;
		Texture(Texture&& other) = default;
		~Texture() override = default;

		Texture(const std::wstring& name) : entity(name)
		{
			for(auto& p : std::filesystem::recursive_directory_iterator(L"./resources/images/" + name))
			{
				if(p.is_regular_file())
				{
					auto category = p.path().parent_path().stem().native();
					auto filename = p.path().stem().native();
					auto storage_name = name + TEXT("_") + category + TEXT("_") + filename;

					m_images[storage_name] = Resource::ResourceManager::load<T>(storage_name, p);
					m_images[storage_name].lock()->load();
				}
			}
		}

		std::weak_ptr<T> get_image(const std::wstring& category, const std::wstring& orientation)
		{
			return m_images[m_name + TEXT("_") + category + TEXT("_") + orientation];
		}

	
	private:
	    std::map<std::wstring, std::weak_ptr<T>> m_images;
	};
}

#endif // TEXTURE_HPP
