#pragma once
#ifndef SECNWINDCHARACTER_HPP
#define SECNWINDCHARACTER_HPP
#include "character.hpp"
#include "EnergyBallProjectile.hpp"
#include "GuidedMissileProjectile.hpp"
#include "math.h"
#include "MissileProjectile.hpp"
#include "MultiEnergyBallProjectile.hpp"

namespace Fortress::Object
{
	class SecwindCharacter final : public ObjectBase::character
	{
	public:
		SecwindCharacter(
			int player_id,
			const std::wstring& name, 
			const Math::Vector2& position, 
			const Math::Vector2& orientation)
			:
			character(
				name,
				L"secwind",
				orientation,
				position,
				{0.0f, 0.0f},
				1.0f,
				{25.0f, 1.0f},
				{},
				ObjectBase::character_full_hp,
				ObjectBase::character_full_mp,
				0.7f)
		{
			initialize();
		}
		SecwindCharacter& operator=(const SecwindCharacter& other) = default;
		SecwindCharacter& operator=(SecwindCharacter&& other) = default;
		SecwindCharacter(const SecwindCharacter& other) = default;
		SecwindCharacter(SecwindCharacter&& other) = default;
		~SecwindCharacter() override = default;

		void initialize() override
		{
			character::initialize();
		}
	protected:
		std::weak_ptr<ObjectBase::projectile> get_main_projectile() override;
		std::weak_ptr<ObjectBase::projectile> get_sub_projectile() override;
	};

	inline std::weak_ptr<ObjectBase::projectile> SecwindCharacter::get_main_projectile()
	{
		return ObjectBase::ObjectManager::create_object<EnergyBallProjectile>(this).lock();
	}

	inline std::weak_ptr<ObjectBase::projectile> SecwindCharacter::get_sub_projectile()
	{
		return ObjectBase::ObjectManager::create_object<MultiEnergyBallProjectile>(this).lock();
	}
}
#endif // SECNWINDCHARACTER_HPP
