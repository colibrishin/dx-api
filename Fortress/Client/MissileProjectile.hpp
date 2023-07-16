#pragma once
#ifndef PRECISONPROJECTILE_HPP
#define PRECISONPROJECTILE_HPP

#include "GifWrapper.hpp"
#include "projectile.hpp"
#include "math.h"
#include "resourceManager.hpp"
#include "Texture.hpp"

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
			2.0f, 
			0.0f, 
			10.0f)
		{
			MissileProjectile::initialize();
		}
		~MissileProjectile() override
		{
			projectile::~projectile();
		}

		void initialize() override;
		virtual void fire(const Math::Vector2& position, const Math::Vector2& velocity, const float charged) override;;

	private:
		Math::Vector2 m_fired_position;
	};

	inline void MissileProjectile::initialize()
	{
		projectile::initialize();
	}

	inline void MissileProjectile::fire(const Math::Vector2& position, const Math::Vector2& velocity,
		const float charged)
	{
		projectile::fire(position, velocity, charged);
	}
}
#endif // PRECISONPROJECTILE_HPP
