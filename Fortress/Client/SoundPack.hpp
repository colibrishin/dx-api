#pragma once
#ifndef SOUNDPACK_HPP
#define SOUNDPACK_HPP

#include "resourceManager.hpp"
#include "sound.hpp"

namespace Fortress
{
	class SoundPack : public Abstract::entity
	{
	public:
		SoundPack() = delete;
		SoundPack& operator=(const SoundPack& other) = default;
		SoundPack& operator=(SoundPack&& other) = default;
		SoundPack(const SoundPack& other) = default;
		SoundPack(SoundPack&& other) = default;
		~SoundPack() override = default;

		SoundPack(const std::wstring& name) : entity(name)
		{
			for(auto& p : std::filesystem::recursive_directory_iterator(L"./resources/sounds/characters/" + name))
			{
				if(p.is_regular_file())
				{
					auto category = p.path().stem().native();
					auto storage_name = name + TEXT("_") + category;

					m_sounds[storage_name] = Resource::ResourceManager::load<Resource::Sound>(storage_name, p);
					m_sounds[storage_name].lock()->load();
				}
			}
		}

		std::weak_ptr<Resource::Sound> get_sound(const std::wstring& category)
		{
			return m_sounds[m_name + TEXT("_") + category];
		}

	
	private:
	    std::map<std::wstring, std::weak_ptr<Resource::Sound>> m_sounds;
	};
}

#endif // SOUNDPACK_HPP
