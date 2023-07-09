#include "projectile.hpp"
#include "character.hpp"

namespace Fortress::ObjectBase
{
	float projectile::get_damage() const
	{
		return m_damage;
	}

	__forceinline void projectile::initialize()
	{
		rigidBody::initialize();
		_known_projectiles.push_back(this);
	}
}
