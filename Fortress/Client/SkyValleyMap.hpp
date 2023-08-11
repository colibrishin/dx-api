#ifndef SKYVALLEYMAP_HPP
#define SKYVALLEYMAP_HPP
#include "BattleScene.h"
#include "CannonCharacter.hpp"
#include "CloudGround.hpp"
#include "MissileCharacter.hpp"
#include "objectManager.hpp"

namespace Fortress::Map
{
	class SkyValleyMap : public Scene::BattleScene
	{
	public:
		SkyValleyMap() : BattleScene(L"SkyValley", {1524.0f, 800.0f}) {}
		void pre_initialize() override;
		void set_bgm() override;
		void set_background_img() override;
		void set_grounds() override;
		void set_characters() override;
		void set_client_character() override;
		bool predicate_OOB(const Math::Vector2& position) override;
		bool movable(const Math::Vector2& position) override;
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

	inline void SkyValleyMap::set_characters()
	{
		m_characters.emplace_back(
			ObjectBase::ObjectManager::create_object<Object::MissileCharacter>(
				0, L"Missile", Math::Vector2{50.0f, 50.0f}, Math::right));
		m_characters.emplace_back(
			ObjectBase::ObjectManager::create_object<Object::CannonCharacter>(
				1, L"Cannon", Math::Vector2{100.0f, 50.0f}, Math::left));
	}

	inline void SkyValleyMap::set_client_character()
	{
		m_self = m_characters.front();
	}

	inline bool SkyValleyMap::predicate_OOB(const Math::Vector2& position)
	{
		return !movable(position);
	}

	inline bool SkyValleyMap::movable(const Math::Vector2& position)
	{
		if(position.get_x() <= m_map_size.get_x() / 2 &&
			position.get_x() >= -m_map_size.get_x() / 2 &&
			position.get_y() <= m_map_size.get_y() &&
			position.get_y() >= 0.0f)
		{
			return true;
		}

		return false;
	}

	inline void SkyValleyMap::set_grounds()
	{
		m_grounds.push_back(
			ObjectBase::ObjectManager::create_object<Object::CloudGround>(
				L"cloudy", Math::Vector2{0.0f, 300.f}));
	}
}
#endif // SKYVALLEYMAP_HPP
