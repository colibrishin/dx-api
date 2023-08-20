#ifndef SKYVALLEYMAP_HPP
#define SKYVALLEYMAP_HPP
#include "../Common/BattleScene.h"
#include "CannonCharacter.hpp"
#include "CloudGround.hpp"
#include "MissileCharacter.hpp"
#include "../Common/objectManager.hpp"

namespace Fortress::Map
{
	class SkyValleyMap final : public Map
	{
	public:
		SkyValleyMap(const Network::GameInitMsg& game_init) : Map(L"SkyValley", game_init)
		{
			Map::initialize();
		}

		void pre_initialize() override;
		void set_bgm() override;
		void set_background_img() override;
		void set_grounds() override;
	};

	inline void SkyValleyMap::pre_initialize()
	{
		Resource::ResourceManager::load<Resource::Sound>(
			L"There's something about supertank", "./resources/sounds/stages/Stage00008.wav");
		Resource::ResourceManager::load<ImageWrapper>(
			L"SkyValley_wirestock", "./resources/images/wirestock_valley.jpg");
	}

	inline void SkyValleyMap::set_bgm()
	{
		m_bgm = Resource::ResourceManager::find<Resource::Sound>(
			L"There's something about supertank");
	}

	inline void SkyValleyMap::set_background_img()
	{
		m_background = Resource::ResourceManager::find<ImageWrapper>(
			L"SkyValley_wirestock");
	}

	inline void SkyValleyMap::set_grounds()
	{
		m_grounds.push_back(
			ObjectBase::ObjectManager::create_object<Object::CloudGround>(
				L"cloudy", Math::Vector2{0.0f, 0.f}));
	}
}
#endif // SKYVALLEYMAP_HPP
