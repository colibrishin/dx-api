#ifndef DESERTMAP_HPP
#define DESERTMAP_HPP
#include "BattleScene.h"
#include "CannonCharacter.hpp"
#include "Catwalk.hpp"
#include "MissileCharacter.hpp"
#include "objectManager.hpp"

namespace Fortress::Map
{
	class DesertMap : public Scene::BattleScene
	{
	public:
		DesertMap() : BattleScene(L"Desert") {}
		void pre_initialize() override;
		void set_bgm() override;
		void set_background_img() override;
		void set_grounds() override;
		void set_characters() override;
		void set_client_character() override;
	};

	inline void DesertMap::pre_initialize()
	{
		Resource::ResourceManager::load<Resource::Sound>(
			L"Egyptian Rule", "./resources/sounds/stages/Stage00015.wav");
		Resource::ResourceManager::load<ImageWrapper>(
			L"Desert_shutterstock", "./resources/images/desert-landscape-shutterstock.jpg");
	}

	inline void DesertMap::set_bgm()
	{
		m_bgm = Resource::ResourceManager::find<Resource::Sound>(
			L"Egyptian Rule");
	}

	inline void DesertMap::set_background_img()
	{
		m_background = Resource::ResourceManager::find<ImageWrapper>(
			L"Desert_shutterstock");
	}

	inline void DesertMap::set_characters()
	{
		m_characters.emplace_back(
			ObjectBase::ObjectManager::create_object<Object::CannonCharacter>(
				0, L"Cannon", Math::Vector2{100.0f, 1.0f}, Math::left));
		m_characters.emplace_back(
			ObjectBase::ObjectManager::create_object<Object::MissileCharacter>(
				1, L"Missile", Math::Vector2{1.0f, 1.0f}, Math::right));
	}

	inline void DesertMap::set_client_character()
	{
		m_self = m_characters.front();
	}

	inline void DesertMap::set_grounds()
	{
		m_grounds.push_back(
			ObjectBase::ObjectManager::create_object<Object::Catwalk>(
				L"left_upper", Math::Vector2{0.0f, 200.0f}, Math::Vector2{100.0f, 100.0f}));
		m_grounds.push_back(
			ObjectBase::ObjectManager::create_object<Object::Ground>(
				L"mid_lower", Math::Vector2{100.0f, 300.0f}, Math::Vector2{100.0f, 100.0f}));
		m_grounds.push_back(
			ObjectBase::ObjectManager::create_object<Object::Catwalk>(
				L"right_upper", Math::Vector2{200.0f, 200.0f}, Math::Vector2{100.0f, 100.0f}));
	}
}
#endif // DESERTMAP_HPP