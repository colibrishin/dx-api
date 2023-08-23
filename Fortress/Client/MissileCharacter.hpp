#pragma once
#ifndef MISSILECHARACTER_HPP
#define MISSILECHARACTER_HPP
#include "ClientCharacter.hpp"
#include "../Common/character.hpp"
#include "GuidedMissileProjectile.hpp"
#include "MissileProjectile.hpp"
#include "../Common/objectManager.hpp"

namespace Fortress::Object
{
	class MissileCharacter final : public Network::Client::Object::ClientCharacter
	{
	public:
		MissileCharacter(
			int player_id,
			const std::wstring& name, 
			const Math::Vector2& position, 
			const Math::Vector2& orientation)
			:
			ClientCharacter(
				player_id,
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
				Property::character_armor_getter(L"missile"))
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

			// half of image size
			set_sprite_offset(L"projectile", L"left", {46.f, 10.0f});
			set_sprite_rotation_offset(L"projectile", L"left", {-46.f, -10.0f});

			character::initialize();
		}

		Network::eCharacterType get_type() const override;
	protected:
		std::weak_ptr<ObjectBase::projectile> get_main_projectile() override;
		std::weak_ptr<ObjectBase::projectile> get_sub_projectile() override;
	};

	inline Network::eCharacterType MissileCharacter::get_type() const
	{
		return Network::eCharacterType::MissileCharacter;
	}

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
