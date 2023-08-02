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
		TeleportItem() : item(L"Teleport")
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
			if(const auto nutshell_check = 
				std::dynamic_pointer_cast<Object::NutShellProjectile>(
					ch->get_current_projectile().lock()))
			{
				if (!ch->get_current_projectile().lock()->is_active())
				{
					ch->m_position = ch->get_current_projectile().lock()->m_position;
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
