#pragma once
#ifndef PRECISIONCANNONPROJECTILE_HPP
#define PRECISIONCANNONPROJECTILE_HPP

#include "GifWrapper.hpp"
#include "projectile.hpp"
#include "math.h"  // NOLINT(modernize-deprecated-headers)
#include "resourceManager.hpp"
#include "Texture.hpp"

namespace Fortress::Object
{
	class PrecisionCannonProjectile final : public ObjectBase::projectile
	{
	public:
		PrecisionCannonProjectile() : projectile(
			L"Precision Cannon Projectile",
			L"cannon",
			{}, 
			Math::identity,
			5.0f,
			200.0f, 
			0.0f, 
			30.0f,
			10,
			1)
		{
			PrecisionCannonProjectile::initialize();
		}

		PrecisionCannonProjectile& operator=(const PrecisionCannonProjectile& other) = default;
		PrecisionCannonProjectile& operator=(PrecisionCannonProjectile&& other) = default;
		PrecisionCannonProjectile(const PrecisionCannonProjectile& other) = default;
		PrecisionCannonProjectile(PrecisionCannonProjectile&& other) = default;
		~PrecisionCannonProjectile() override = default;

		void initialize() override;
	};

	inline void PrecisionCannonProjectile::initialize()
	{
		projectile::initialize();
	}
}
#endif // PRECISIONCANNONPROJECTILE_HPP
