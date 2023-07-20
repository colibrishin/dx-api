#pragma once
#ifndef CANNONPROJECTILE_HPP
#define CANNONPROJECTILE_HPP

#include "GifWrapper.hpp"
#include "projectile.hpp"
#include "math.h"  // NOLINT(modernize-deprecated-headers)
#include "resourceManager.hpp"
#include "Texture.hpp"

namespace Fortress::Object
{
	class CannonProjectile final : public ObjectBase::projectile
	{
	public:
		CannonProjectile() : projectile(
			L"Cannon Projectile",
			L"cannon",
			{}, 
			Math::identity,
			5.0f,
			200.0f, 
			0.0f, 
			10.0f,
			50,
			1)
		{
			CannonProjectile::initialize();
		}

		CannonProjectile& operator=(const CannonProjectile& other) = default;
		CannonProjectile& operator=(CannonProjectile&& other) = default;
		CannonProjectile(const CannonProjectile& other) = default;
		CannonProjectile(CannonProjectile&& other) = default;
		~CannonProjectile() override = default;

		void initialize() override;

	private:
		Math::Vector2 m_fired_position;
	};

	inline void CannonProjectile::initialize()
	{
		projectile::initialize();
	}
}
#endif // CANNONPROJECTILE_HPP
