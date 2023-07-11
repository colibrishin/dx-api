#include "projectile.hpp"
#include "character.hpp"
#include "ground.hpp"

namespace Fortress::ObjectBase
{
	void projectile::on_collision(rigidBody* other)
	{
		bool hit = false;

		if (auto gr = dynamic_cast<Object::Ground*>(other))
		{
			// @todo: dig a hole into the Ground.
			Debug::Log(L"Projectile hits the Ground");
			hit = true;
		}

		if (const auto ch = dynamic_cast<character*>(other))
		{
			Debug::Log(L"Projectile hits the character");
			hit = true;
			ch->hit(this);
		}

		if (hit)
		{
			set_disabled();
			// @todo: do a "thing" if a projectile is a explosive.
		}

		rigidBody::on_collision(other);
	}

	float projectile::get_damage() const
	{
		return m_damage;
	}

	__forceinline void projectile::initialize()
	{
		rigidBody::initialize();
	}
}
