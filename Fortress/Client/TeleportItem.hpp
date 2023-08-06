#ifndef TELEPORTITEM_HPP
#define TELEPORTITEM_HPP
#include "item.hpp"
#include "projectile.hpp"
#include "NutshellProjectile.hpp"

namespace Fortress::Item
{
	class TeleportItem : public Object::item
	{
	public:
		TeleportItem() : item(L"Teleport", false)
		{
			TeleportItem::initialize();
		}

		void initialize() override;
		virtual void update(const std::weak_ptr<ObjectBase::character> owner) override;
		~TeleportItem() override = default;
	};

	inline void TeleportItem::initialize()
	{
	}

	inline void TeleportItem::update(const std::weak_ptr<ObjectBase::character> owner)
	{
		if (const auto ch = owner.lock())
		{
			if (ch->get_projectile_type() == eProjectileType::Nutshell)
			{
				if(!ch->is_projectile_active())
				{
					const auto prj = *ch->get_projectiles().begin();

					// @todo: teleport to the outside of the map should be handled.
					ch->m_position = prj.lock()->m_position;
					ch->unequip_nutshell();
					set_ended();
					item::update(owner);
				}
			}	
			else
			{
				ch->equip_nutshell();
				ch->shoot();
			}
		}
	}
}
#endif // DOUBLESHOTITEM_HPP
