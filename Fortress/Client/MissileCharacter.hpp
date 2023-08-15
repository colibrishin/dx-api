#pragma once
#ifndef MISSILECHARACTER_HPP
#define MISSILECHARACTER_HPP
#include "character.hpp"
#include "GifWrapper.h"
#include "GuidedMissileProjectile.hpp"
#include "math.h"
#include "MissileProjectile.hpp"
#include "objectManager.hpp"
#include "resourceManager.hpp"

namespace Fortress::Object
{
	class MissileCharacter final : public ObjectBase::character
	{
	public:
		MissileCharacter(
			int player_id,
			const std::wstring& name, 
			const Math::Vector2& position, 
			const Math::Vector2& orientation)
			:
			character(
				name,
				L"missile",
				orientation,
				position,
				{0.0f, 0.0f},
				1.0f,
				{20.0f, 1.0f},
				{},
				ObjectBase::character_full_hp,
				ObjectBase::character_full_mp,
				1.0f)
		{
			initialize();
		}
		MissileCharacter& operator=(const MissileCharacter& other) = default;
		MissileCharacter& operator=(MissileCharacter&& other) = default;
		MissileCharacter(const MissileCharacter& other) = default;
		MissileCharacter(MissileCharacter&& other) = default;
		~MissileCharacter() override = default;

		void initialize() override
		{
			set_sprite_offset(L"fire", L"right", {0, 10.0f});
			set_sprite_offset(L"fire", L"left", {45.0f, 10.0f});
			set_sprite_offset(L"charging", L"right", {0, 10.0f});
			set_sprite_offset(L"charging", L"left", {45.0f, 10.0f});
			set_sprite_offset(L"idle", L"left", {15.0f, 0.0f});

			set_sprite_offset(L"projectile", L"left", {50.5f, 0.0f});
			set_sprite_rotation_offset(L"projectile", L"left", {-50.5f, 0.0f});

			character::initialize();
		}
	protected:
		std::weak_ptr<ObjectBase::projectile> get_main_projectile() override;
		std::weak_ptr<ObjectBase::projectile> get_sub_projectile() override;
	};

	inline std::weak_ptr<ObjectBase::projectile> MissileCharacter::get_main_projectile()
	{
		return ObjectBase::ObjectManager::create_object<MissileProjectile>(this).lock();
	}

	inline std::weak_ptr<ObjectBase::projectile> MissileCharacter::get_sub_projectile()
	{
		return ObjectBase::ObjectManager::create_object<GuidedMissileProjectile>(this).lock();
	}
}
#endif // MISSILECHARACTER_HPP
