#include "character.hpp"
#include "projectile.hpp"

namespace Fortress::ObjectBase
{
	void character::hit(const projectile* p)
	{
		m_hp -= p->get_damage();
	}
}
