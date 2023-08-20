#pragma once
#ifndef CANNONCHARACTER_HPP
#define CANNONCHARACTER_HPP
#include "Client.h"
#include "CannonProjectile.hpp"
#include "../Common/character.hpp"
#include "../Common/objectManager.hpp"
#include "PrecisionCannonProjectile.hpp"

namespace Fortress::Object
{
	class CannonCharacter final : public ObjectBase::character
	{
	public:
		CannonCharacter(
			int player_id,
			const std::wstring& name, 
			const Math::Vector2& position, 
			const Math::Vector2& orientation)
			:character(
				player_id,
				name,
				L"cannon",
				orientation,
				position,
				{0.0f, 0.0f},
				1.0f,
				{20.0f, 1.0f},
				{},
				ObjectBase::character_full_hp,
				ObjectBase::character_full_mp,
				0.5f)
		{
			initialize();
		}
		CannonCharacter& operator=(const CannonCharacter& other) = default;
		CannonCharacter& operator=(CannonCharacter&& other) = default;
		CannonCharacter(const CannonCharacter& other) = default;
		CannonCharacter(CannonCharacter&& other) = default;
		~CannonCharacter() override = default;

		void initialize() override
		{
			set_sprite_offset(L"fire", L"right", {0, 10.0f});
			set_sprite_offset(L"fire", L"left", {0, 10.0f});
			set_sprite_offset(L"fire_sub", L"right", {0, 10.0f});
			set_sprite_offset(L"fire_sub", L"left", {0, 10.0f});
			set_sprite_offset(L"charging", L"right", {0, 10.0f});
			set_sprite_offset(L"charging", L"left", {0, 10.0f});

			character::initialize();
		}

	protected:
		std::weak_ptr<ObjectBase::projectile> get_main_projectile() override;
		std::weak_ptr<ObjectBase::projectile> get_sub_projectile() override;
	};

	inline std::weak_ptr<ObjectBase::projectile> CannonCharacter::get_main_projectile()
	{
		return ObjectBase::ObjectManager::create_object<CannonProjectile>(this).lock();
	}

	inline std::weak_ptr<ObjectBase::projectile> CannonCharacter::get_sub_projectile()
	{
		return ObjectBase::ObjectManager::create_object<PrecisionCannonProjectile>(this).lock();
	}
}

#endif // CANNONCHARACTER_HPP
