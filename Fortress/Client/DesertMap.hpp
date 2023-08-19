#ifndef DESERTMAP_HPP
#define DESERTMAP_HPP
#include "../Common/BattleScene.h"
#include "CannonCharacter.hpp"
#include "EgyptGround.hpp"
#include "EgyptStairway.hpp"
#include "MissileCharacter.hpp"
#include "../Common/objectManager.hpp"
#include "SecwindCharacter.hpp"

namespace Fortress::Map
{
	class DesertMap final : public Scene::BattleScene
	{
	public:
		DesertMap() : BattleScene(L"Desert")
		{
			BattleScene::initialize();
		}
		void pre_initialize() override;
		void set_bgm() override;
		void set_background_img() override;
		void set_grounds() override;
		void set_characters() override;
		void set_client_character() override;
	};

	inline void DesertMap::pre_initialize()
	{
	}

	inline void DesertMap::set_bgm()
	{
		m_bgm = Resource::ResourceManager::load<Resource::Sound>(
			L"Egyptian Rule", "./resources/sounds/stages/Stage00015.wav");
	}

	inline void DesertMap::set_background_img()
	{
		m_background = Resource::ResourceManager::load<ImageWrapper>(
			L"Desert_shutterstock", "./resources/images/desert-landscape-shutterstock.jpg");
	}

	inline void DesertMap::set_characters()
	{
		m_characters.emplace_back(
			ObjectBase::ObjectManager::create_object<Object::SecwindCharacter>(
				0, L"Secwind", Math::Vector2{350.0f, 1.0f}, Math::left));
		m_characters.emplace_back(
			ObjectBase::ObjectManager::create_object<Object::MissileCharacter>(
				1, L"Missile", Math::Vector2{-150.0f, 1.0f}, Math::right));
	}

	inline void DesertMap::set_client_character()
	{
		m_self = m_characters.front();
	}

	inline void DesertMap::set_grounds()
	{
		m_grounds.push_back(
			ObjectBase::ObjectManager::create_object<Object::EgyptStairway>(
				L"left_upper", 
				Math::Vector2{-300.0f, 0.0f}, 
				Math::Vector2{500.0f, 100.0f}, Math::left));
		m_grounds.push_back(
			ObjectBase::ObjectManager::create_object<Object::EgyptGround>(
				L"mid_lower", 
				Math::Vector2{0.0f, 100.0f}, 
				Math::Vector2{500.0f, 100.0f}));
		m_grounds.push_back(
			ObjectBase::ObjectManager::create_object<Object::EgyptStairway>(
				L"right_upper", 
				Math::Vector2{300.0f, 0.0f},
				Math::Vector2{500.0f, 100.0f}, Math::right));
	}
}
#endif // DESERTMAP_HPP
