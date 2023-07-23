#pragma once
#ifndef MISSILEPROJECTILE_HPP
#define MISSILEPROJECTILE_HPP

#include "GifWrapper.hpp"
#include "projectile.hpp"
#include "math.h"

namespace Fortress::Object
{
	class MissileProjectile final : public ObjectBase::projectile
	{
	public:
		MissileProjectile() : projectile(
			L"Precision Projectile",
			L"missile",
			{}, 
			Math::identity,
			5.0f,
			2.0f, 
			0.0f, 
			10.0f,
			30,
			2)
		{
			MissileProjectile::initialize();
		}

		MissileProjectile& operator=(const MissileProjectile& other) = default;
		MissileProjectile& operator=(MissileProjectile&& other) = default;
		MissileProjectile(const MissileProjectile& other) = default;
		MissileProjectile(MissileProjectile&& other) = default;
		~MissileProjectile() override = default;

		void initialize() override;

	private:
		Math::Vector2 m_fired_position;
	};

	inline void MissileProjectile::initialize()
	{
		projectile::initialize();
	}
}
#endif // CANNONPROJECTILE_HPP
